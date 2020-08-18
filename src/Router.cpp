#include "Router.hpp"



static std::string GetRouterName(std::int32_t id)
{
	return (std::stringstream() << "Router[" << std::setfill('0') << std::setw(3) << id << "]").str();
}


Router::Router(sc_module_name, const SimulationTimer& timer, std::int32_t id, size_t relays, std::int32_t max_buffer_size) :
	stats(timer, id, Relays.size()), power(timer),
	LocalID(id),
	Relays("RouterRelays", relays + 1),
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
			Relays[i].rx_buffer_full_status.write(false);
		}

		for (size_t i = 0; i < Relays.size(); i++)
			Relays[i].free_slots.write(Relays[i].buffer.GetMaxBufferSize());
		return;
	}

	TXProcess();
	RXProcess();

	start_from_port = (start_from_port + 1) % Relays.size();

	for (std::int32_t i = 0; i < Relays.size(); i++)
		Relays[i].free_slots.write(Relays[i].buffer.GetCurrentFreeSlots());

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

Router::Router(const SimulationTimer& timer, std::int32_t id, size_t relays, std::int32_t max_buffer_size) : 
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