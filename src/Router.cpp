/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the router
 */
#include "Router.h"



int32_t Router::PerformRoute(const RouteData& route_data)
{
	if (route_data.dst_id == local_id) return LocalRelayID;

	power.routing();
	power.selection();
	return Strategy.Apply(*this, Algorithm.Route(*this, route_data), route_data);
}
void Router::PerCycleProcess()
{
	if (reset.read()) 
	{
		for (size_t i = 0; i < Relays.size(); i++)
			Relays[i].free_slots.write(Relays[i].buffer.GetMaxBufferSize());
	}
	else 
	{
		Strategy.PerCycleProcess(*this);

		power.leakageRouter();
		for (int i = 0; i < Relays.size(); i++)
		{
			power.leakageBufferRouter();
			power.leakageLinkRouter2Router();
			stats.UpdateBufferLoad(sc_time_stamp().to_double() / GlobalParams::clock_period_ps, i, Relays[i].buffer.GetLoad());
		}
	}
}
void Router::TXProcess()
{
	if (reset.read())
	{
		// Clear outputs and indexes of transmitting protocol
		for (size_t i = 0; i < Relays.size(); i++)
		{
			Relays[i].tx_req.write(0);
			Relays[i].tx_current_level = 0;
		}
	}
	else
	{
		// 1st phase: Reservation
		for (int j = 0; j < Relays.size(); j++)
		{
			int i = (start_from_port + j) % Relays.size();
			Relay& rel = Relays[i];

			// Uncomment to enable deadlock checking on buffers. 
			// Please also set the appropriate threshold.
			// buffer[i].deadlockCheck();
			if (!rel.buffer.IsEmpty())
			{
				Flit flit = rel.buffer.Front();
				power.bufferRouterFront();

				if (flit.flit_type == FLIT_TYPE_HEAD)
				{
					// prepare data for routing
					RouteData route_data;
					route_data.hop_no = flit.hop_no;
					route_data.current_id = local_id;
					route_data.src_id = flit.src_id;
					route_data.dst_id = flit.dst_id;
					route_data.dir_in = i;
					route_data.sequence_length = flit.sequence_length;
				
					// TODO: see PER POSTERI (adaptive routing should not recompute route if already reserved)
					int32_t out = PerformRoute(route_data);
					if (out < 0) continue;

					if (!reservation_table.Reserved(out))
						reservation_table.Reserve(i, out);
				}
			}
		}

		start_from_port = (start_from_port + 1) % Relays.size();

		// 2nd phase: Forwarding
		for (int32_t i = 0; i < Relays.size(); i++)
		{
			Relay& rel = Relays[i];

			if (!rel.buffer.IsEmpty())
			{
				int32_t res = reservation_table.Reservation(i);
				if (res < 0) continue;

				int32_t out_port = res;
				Relay& out_rel = Relays[out_port];

				Flit flit = rel.buffer.Front();

				bool buf_stat = out_rel.tx_buffer_full_status.read();

				if ((out_rel.tx_current_level == out_rel.tx_ack.read()) && (buf_stat == false))
				{
					out_rel.tx_flit.write(flit);
					out_rel.tx_current_level = 1 - out_rel.tx_current_level;
					out_rel.tx_req.write(out_rel.tx_current_level);
					rel.buffer.Pop();

					if (flit.flit_type == FLIT_TYPE_TAIL)
						reservation_table.Release(i);

					/* Power & Stats ------------------------------------------------- */
					stats.UpdateBufferPopOrEmptyTime(i, sc_time_stamp().to_double() / GlobalParams::clock_period_ps);

					power.r2rLink();

					power.bufferRouterPop();
					power.crossBar();

					if (out_port == LocalRelayID)
					{
						power.networkInterface();
						//LOG << "Consumed flit " << flit << endl;
						stats.receivedFlit(sc_time_stamp().to_double() / GlobalParams::clock_period_ps, flit);
						if (GlobalParams::max_volume_to_be_drained)
						{
							if (drained_volume >= GlobalParams::max_volume_to_be_drained) sc_stop();
							else drained_volume++;
						}
					}
					else if (i != LocalRelayID) routed_flits++;
					/* End Power & Stats ------------------------------------------------- */
				}
			}
			else stats.UpdateBufferPopOrEmptyTime(i, sc_time_stamp().to_double() / GlobalParams::clock_period_ps);
		} // for loop directions
	}
}
void Router::RXProcess()
{
	if (reset.read()) 
	{
		// Clear outputs and indexes of receiving protocol
		for (size_t i = 0; i < Relays.size(); i++)
		{
			Relays[i].rx_ack.write(0);
			Relays[i].rx_current_level = 0;
			Relays[i].rx_buffer_full_status.write(false);
		}

		routed_flits = 0;
	}
	else
	{
		// This process simply sees a flow of incoming flits. All arbitration
		// and wormhole related issues are addressed in the txProcess()
		//assert(false);
		for (size_t i = 0; i < Relays.size(); i++)
		{
			Relay& rel = Relays[i];
			// To accept a new flit, the following conditions must match:
			// 1) there is an incoming request
			// 2) there is a free slot in the input buffer of direction i

			if (rel.rx_req.read() == 1 - rel.rx_current_level)
			{
				Flit received_flit = rel.rx_flit.read();

				int vc = received_flit.vc_id;

				if (!rel.buffer.IsFull())
				{
					// Store the incoming flit in the circular buffer
					rel.buffer.Push(received_flit);
					//LOG << " Flit " << received_flit << " collected from Input[" << i << "][" << vc << "]" << endl;

					power.bufferRouterPush();

					// Negate the old value for Alternating Bit Protocol (ABP)
					rel.rx_current_level = 1 - rel.rx_current_level;

					// if a new flit is injected from local PE
					if (received_flit.src_id == local_id)
					{
						stats.AcceptFlit(sc_time_stamp().to_double() / GlobalParams::clock_period_ps);
						power.networkInterface();
					}
				}
				else  // buffer full
				{
					// should not happen with the new TBufferFullStatus control signals    
					// except for flit coming from local PE, which don't use it 
					//LOG << " Flit " << received_flit << " buffer full Input[" << i << "][" << vc << "]" << endl;
					assert(i == LocalRelayID);
				}

			}
			rel.rx_ack.write(rel.rx_current_level);
			// updates the mask of VCs to prevent incoming data on full buffers
			bool bfs = rel.buffer.IsFull();
			rel.rx_buffer_full_status.write(bfs);
		}
	}
}
void Router::Update()
{
	TXProcess();
	RXProcess();
	PerCycleProcess();
}

Router::Router(sc_module_name, int32_t id, size_t relays, double warm_up_time, uint32_t max_buffer_size,
	RoutingAlgorithm& alg, SelectionStrategy& sel, RoutingTable& grt) : Relays("RouterRelays", relays + 1), 
	LocalRelay(Relays[relays]), LocalRelayID(relays), Algorithm(alg), Strategy(sel), 
	reservation_table(Relays.size()), stats(id, warm_up_time, Relays.size())
{
	if (grt.IsValid()) routing_table = grt[id];
	SC_METHOD(Update);
	sensitive << reset << clock.pos();

	local_id = id;

	start_from_port = LocalRelayID;

	for (size_t i = 0; i < Relays.size(); i++)
	{
		Relays[i].buffer.SetMaxBufferSize(max_buffer_size);
		Relays[i].buffer.SetLabel(std::string(name()) + "->buffer[" + i_to_string(i) + "]");
	}
}

uint32_t Router::GetRoutedFlits() const
{
	return routed_flits;
}
bool Router::InCongestion() const
{
	for (size_t i = 0; i < Relays.size() - 1; i++)
	{
		if (Relays[i].free_slots_neighbor == NOT_VALID) continue;

		int flits = GlobalParams::buffer_depth - Relays[i].free_slots_neighbor;
		if (flits > (int)(GlobalParams::buffer_depth * GlobalParams::dyad_threshold)) return true;
	}

	return false;
}