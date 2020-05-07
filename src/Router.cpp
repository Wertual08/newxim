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



std::vector<int32_t> Router::RoutingFunction(const RouteData& route_data)
{
	// TODO: fix all the deprecated verbose mode logs
	if (GlobalParams::verbose_mode > VERBOSE_OFF)
		LOG << "Wired routing for dst = " << route_data.dst_id << endl;

	return Algorithm.Route(*this, route_data);
}
int Router::SelectionFunction(const std::vector<int32_t>& directions, const RouteData& route_data)
{
	// not so elegant but fast escape ;) SHIT
	// if (directions.size() == 1) return directions[0];

	return Strategy.Apply(*this, directions, route_data);
}
int32_t Router::PerformRoute(const RouteData& route_data)
{
	if (route_data.dst_id == local_id) return LocalRelayID;

	power.routing();
	std::vector<int> candidate_channels = RoutingFunction(route_data);

	power.selection();
	return SelectionFunction(candidate_channels, route_data);
}
void Router::PerCycleProcess()
{
	if (reset.read()) 
	{
		for (size_t i = 0; i < Relays.size(); i++)
			Relays[i].free_slots.write(Relays[i].buffer[DEFAULT_VC].GetMaxBufferSize());
	}
	else 
	{
		Strategy.PerCycleProcess(*this);

		power.leakageRouter();
		for (int i = 0; i < Relays.size(); i++)
		{
			for (int vc = 0; vc < GlobalParams::n_virtual_channels; vc++)
			{
				power.leakageBufferRouter();
				power.leakageLinkRouter2Router();
			}
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
			if (local_id == 48 && sc_time_stamp().to_double() / GlobalParams::clock_period_ps == 1017)
			{
				int k = 0;
			}
			for (int k = 0; k < GlobalParams::n_virtual_channels; k++)
			{
				int vc = (rel.start_from_vc + k) % GlobalParams::n_virtual_channels;

				// Uncomment to enable deadlock checking on buffers. 
				// Please also set the appropriate threshold.
				// buffer[i].deadlockCheck();
				if (!rel.buffer[vc].IsEmpty())
				{
					Flit flit = rel.buffer[vc].Front();
					power.bufferRouterFront();

					if (flit.flit_type == FLIT_TYPE_HEAD)
					{
						// prepare data for routing
						RouteData route_data;
						route_data.current_id = local_id;
						//LOG<< "current_id= "<< route_data.current_id <<" for sending " << flit << endl;
						route_data.src_id = flit.src_id;
						route_data.dst_id = flit.dst_id;
						route_data.dir_in = i;
						route_data.vc_id = flit.vc_id;
						route_data.sequence_length = flit.sequence_length;

						// TODO: see PER POSTERI (adaptive routing should not recompute route if already reserved)
						int32_t o = PerformRoute(route_data);
						if (o < 0) continue;

						TReservation r;
						r.input = i;
						r.vc = vc;

						LOG << " checking availability of Output[" << o << "] for Input[" << i << "][" << vc << "] flit " << flit << endl;

						int rt_status = reservation_table.checkReservation(r, o);

						if (rt_status == ReservationTable::Available)
						{
							LOG << " reserving direction " << o << " for flit " << flit << endl;
							reservation_table.reserve(r, o);
						}
						else if (rt_status == ReservationTable::AlreadySame)
						{
							LOG << " ReservationTable::AlreadySame reserved direction " << o << " for flit " << flit << endl;
						}
						else if (rt_status == ReservationTable::OutVCBusy)
						{
							LOG << " ReservationTable::OutVCBusy reservation direction " << o << " for flit " << flit << endl;
						}
						else if (rt_status == ReservationTable::AlreadyOtherOut)
						{
							LOG << "ReservationTable::AlreadyOtherOut: another output previously reserved for the same flit " << endl;
						}
						else assert(false); // no meaningful status here
					}
				}
			}
			rel.start_from_vc = (rel.start_from_vc + 1) % GlobalParams::n_virtual_channels;
		}

		start_from_port = (start_from_port + 1) % Relays.size();

		// 2nd phase: Forwarding
		//if (local_id==6) LOG<<"*TX*****local_id="<<local_id<<"__ack_tx[0]= "<<ack_tx[0].read()<<endl;
		for (int i = 0; i < Relays.size(); i++)
		{
			Relay& rel = Relays[i];
			
			auto reservations = reservation_table.getReservations(i);

			if (reservations.size() != 0)
			{
				int rnd_idx = rand() % reservations.size();

				int o = reservations[rnd_idx].first;
				Relay& orel = Relays[o];

				int vc = reservations[rnd_idx].second;
				// LOG<< "found reservation from input= " << i << "_to output= "<<o<<endl;
				 // can happen
				if (!rel.buffer[vc].IsEmpty())
				{
					// power contribution already computed in 1st phase
					Flit flit = rel.buffer[vc].Front();
					//LOG<< "*****TX***Direction= "<<i<< "************"<<endl;
					//LOG<<"_cl_tx="<<current_level_tx[o]<<"req_tx="<<req_tx[o].read()<<" _ack= "<<ack_tx[o].read()<< endl;

					auto buf_stat = orel.tx_buffer_full_status.read();
					if ((orel.tx_current_level == orel.tx_ack.read()) &&
						(buf_stat.mask[vc] == false))
					{
						//if (GlobalParams::verbose_mode > VERBOSE_OFF) 
						LOG << "Input[" << i << "][" << vc << "] forwarded to Output[" << o << "], flit: " << flit << endl;

						orel.tx_flit.write(flit);
						orel.tx_current_level = 1 - orel.tx_current_level;
						orel.tx_req.write(orel.tx_current_level);
						rel.buffer[vc].Pop();

						if (flit.flit_type == FLIT_TYPE_TAIL)
						{
							TReservation r;
							r.input = i;
							r.vc = vc;
							reservation_table.release(r, o);
						}

						/* Power & Stats ------------------------------------------------- */
						power.r2rLink();

						power.bufferRouterPop();
						power.crossBar();

						if (o == LocalRelayID)
						{
							power.networkInterface();
							LOG << "Consumed flit " << flit << endl;
							stats.receivedFlit(sc_time_stamp().to_double() / GlobalParams::clock_period_ps, flit);
							if (GlobalParams::max_volume_to_be_drained)
							{
								if (drained_volume >= GlobalParams::max_volume_to_be_drained)
									sc_stop();
								else
								{
									drained_volume++;
								}
							}
						}
						else if (i != LocalRelayID) // not generated locally
							routed_flits++;
						/* End Power & Stats ------------------------------------------------- */
					   //LOG<<"END_OK_cl_tx="<<current_level_tx[o]<<"_req_tx="<<req_tx[o].read()<<" _ack= "<<ack_tx[o].read()<< endl;
					}
					else
					{
						LOG << " Cannot forward Input[" << i << "][" << vc << "] to Output[" << o << "], flit: " << flit << endl;
						//LOG << " **DEBUG APB: current_level_tx: " << current_level_tx[o] << " ack_tx: " << ack_tx[o].read() << endl;
						LOG << " **DEBUG buffer_full_status_tx " << orel.tx_buffer_full_status.read().mask[vc] << endl;

						//LOG<<"END_NO_cl_tx="<<current_level_tx[o]<<"_req_tx="<<req_tx[o].read()<<" _ack= "<<ack_tx[o].read()<< endl;
						  /*
						  if (flit.flit_type == FLIT_TYPE_HEAD)
						  reservation_table.release(i,flit.vc_id,o);
						  */
					}
				}
			} // if not reserved 
		   // else LOG << "we have no reservation for direction " << i << endl;
		} // for loop directions

		if ((int)(sc_time_stamp().to_double() / GlobalParams::clock_period_ps) % 2 == 0)
			reservation_table.updateIndex();
	}
}
void Router::RXProcess()
{
	if (reset.read()) 
	{
		TBufferFullStatus bfs;
		// Clear outputs and indexes of receiving protocol
		for (size_t i = 0; i < Relays.size(); i++)
		{
			Relays[i].rx_ack.write(0);
			Relays[i].rx_current_level = 0;
			Relays[i].rx_buffer_full_status.write(bfs);
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
			//LOG<<"****RX****DIRECTION ="<<i<<  endl;

			if (rel.rx_req.read() == 1 - rel.rx_current_level)
			{
				Flit received_flit = rel.rx_flit.read();
				//LOG<<"request opposite to the current_level, reading flit "<<received_flit<<endl;

				int vc = received_flit.vc_id;

				if (!rel.buffer[vc].IsFull())
				{
					// Store the incoming flit in the circular buffer
					rel.buffer[vc].Push(received_flit);
					LOG << " Flit " << received_flit << " collected from Input[" << i << "][" << vc << "]" << endl;

					power.bufferRouterPush();

					// Negate the old value for Alternating Bit Protocol (ABP)
					//LOG<<"INVERTING CL FROM "<< current_level_rx[i]<< " TO "<<  1 - current_level_rx[i]<<endl;
					rel.rx_current_level = 1 - rel.rx_current_level;

					// if a new flit is injected from local PE
					if (received_flit.src_id == local_id)
						power.networkInterface();
				}
				else  // buffer full
				{
					// should not happen with the new TBufferFullStatus control signals    
					// except for flit coming from local PE, which don't use it 
					LOG << " Flit " << received_flit << " buffer full Input[" << i << "][" << vc << "]" << endl;
					assert(i == LocalRelayID);
				}

			}
			rel.rx_ack.write(rel.rx_current_level);
			// updates the mask of VCs to prevent incoming data on full buffers
			TBufferFullStatus bfs;
			for (int vc = 0; vc < GlobalParams::n_virtual_channels; vc++)
				bfs.mask[vc] = rel.buffer[vc].IsFull();
			rel.rx_buffer_full_status.write(bfs);
		}
	}
}
void Router::Update()
{
	//if (!reset.read() && sc_time_stamp().to_double() / GlobalParams::clock_period_ps - GlobalParams::reset_time > 4050)
	//{
	//	for (auto& rel : Relays) rel.buffer->Print();
	//	reservation_table.print();
	//}

	TXProcess();
	RXProcess();
	PerCycleProcess();
}

Router::Router(sc_module_name, int32_t id, size_t relays, double warm_up_time, uint32_t max_buffer_size,
	RoutingAlgorithm& alg, SelectionStrategy& sel, RoutingTable& grt) : Relays("RouterRelays", relays + 1), 
	LocalRelay(Relays[relays]), LocalRelayID(relays), 
	Algorithm(alg), Strategy(sel)
{
	if (grt.IsValid()) routing_table = grt[id];
	SC_METHOD(Update);
	sensitive << reset << clock.pos();

	local_id = id;
	stats.configure(id, warm_up_time);

	start_from_port = LocalRelayID;

	reservation_table.setSize(Relays.size());

	for (size_t i = 0; i < Relays.size(); i++)
	{
		for (int vc = 0; vc < GlobalParams::n_virtual_channels; vc++)
		{
			Relays[i].buffer[vc].SetMaxBufferSize(max_buffer_size);
			Relays[i].buffer[vc].SetLabel(std::string(name()) + "->buffer[" + i_to_string(i) + "]");
		}
		Relays[i].start_from_vc = 0;
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
void Router::ShowBuffersStats(std::ostream& out) const
{
	for (int i = 0; i < Relays.size(); i++)
		for (int vc = 0; vc < GlobalParams::n_virtual_channels; vc++)
			Relays[i].buffer[vc].ShowStats(out);
}