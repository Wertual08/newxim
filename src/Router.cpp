#include "Router.hpp"



static std::string GetRouterName(std::int32_t id)
{
	return (std::stringstream() << "Router[" << std::setfill('0') << std::setw(3) << id << "]").str();
}


Router::Router(sc_module_name, const SimulationTimer& timer, std::int32_t id, std::size_t relays, std::int32_t max_buffer_size) :
	stats(timer, id, Relays.size()), power(timer),
	LocalID(id),
	Relays(relays + 1),
	LocalRelay(Relays[relays]),
	LocalRelayID(relays),
	Routing(nullptr),
	Selection(nullptr)
{
	SC_METHOD(Update);
	sensitive << reset << clock.pos();

	start_from_port = LocalRelayID;

	for (size_t i = 0; i < Relays.size(); i++)
		Relays[i].buffer.SetMaxBufferSize(max_buffer_size);
}

void Router::Update()
{
	if (reset.read())
	{
		// Clear outputs and indexes of transmitting protocol
		for (size_t i = 0; i < Relays.size(); i++)
		{
			Relays[i].tx_req.write(0);
			Relays[i].tx_current_level = 0;
		}

		// Clear outputs and indexes of receiving protocol
		for (size_t i = 0; i < Relays.size(); i++)
		{
			Relays[i].rx_ack.write(0);
			Relays[i].rx_current_level = 0;
			Relays[i].free_slots.write(Relays[i].buffer.GetMaxBufferSize());
		}

		return;
	}

	TXProcess();
	RXProcess();

	start_from_port = (start_from_port + 1) % static_cast<std::int32_t>(Relays.size());

	power.leakageRouter();
	for (int i = 0; i < Relays.size(); i++)
	{
		power.leakageBufferRouter();
		power.leakageLinkRouter2Router();
		stats.UpdateBufferLoad(i, Relays[i].buffer.GetLoad());
	}
}

std::int32_t Router::PerformRoute(const RouteData& route_data)
{
	if (route_data.dst_id == LocalID) return LocalRelayID;

	power.routing();
	power.selection();
	return Selection->Apply(*this, Routing->Route(*this, route_data), route_data);
}
void Router::Route(std::int32_t in_port, std::int32_t out_port)
{
	Relay& in_relay = Relays[in_port];
	Relay& out_relay = Relays[out_port];
	bool buf_stat = out_relay.free_slots_neighbor.read();

	if (out_relay.tx_current_level == out_relay.tx_ack.read() && buf_stat)
	{
		Flit flit = in_relay.buffer.Pop();
		flit.hop_no++;
		out_relay.tx_flit.write(flit);
		out_relay.tx_current_level = 1 - out_relay.tx_current_level;
		out_relay.tx_req.write(out_relay.tx_current_level);

		/* Power & Stats ------------------------------------------------- */
		stats.UpdateBufferPopOrEmptyTime(in_port);

		power.r2rLink();

		power.bufferRouterPop();
		power.crossBar();

		if (out_port == LocalRelayID)
		{
			power.networkInterface();
			stats.receivedFlit(flit);
		}
		/* End Power & Stats ------------------------------------------------- */
	}
}

void Router::RXProcess()
{
	// This process simply sees a flow of incoming flits. All arbitration
	// and wormhole related issues are addressed in the txProcess()
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

				power.bufferRouterPush();

				// Negate the old value for Alternating Bit Protocol (ABP)
				rel.rx_current_level = 1 - rel.rx_current_level;

				// if a new flit is injected from local PE
				if (received_flit.src_id == LocalID)
				{
					stats.AcceptFlit();
					power.networkInterface();
				}
			}
			else  // buffer full
			{
				// should not happen with the new TBufferFullStatus control signals    
				// except for flit coming from local PE, which don't use it 

				assert(i == LocalRelayID);
			}

		}
		rel.rx_ack.write(rel.rx_current_level);
		// updates the mask of VCs to prevent incoming data on full buffers
		rel.free_slots.write(rel.buffer.GetCurrentFreeSlots());
	}
}

Router::Router(const SimulationTimer& timer, std::int32_t id, std::size_t relays, std::int32_t max_buffer_size) :
	Router(GetRouterName(id).c_str(), timer, id, relays, max_buffer_size)
{
}
void Router::SetRoutingAlgorithm(const RoutingAlgorithm& alg)
{
	Routing = &alg;
}
void Router::SetSelectionStrategy(const SelectionStrategy& sel)
{
	Selection = &sel;
}