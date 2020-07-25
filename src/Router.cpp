#include "Router.hpp"



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

	Strategy.PerCycleProcess(*this);

	power.leakageRouter();
	for (int i = 0; i < Relays.size(); i++)
	{
		power.leakageBufferRouter();
		power.leakageLinkRouter2Router();
		stats.UpdateBufferLoad(sc_time_stamp().to_double() / GlobalParams::clock_period_ps, i, Relays[i].buffer.GetLoad());
	}
}

int32_t Router::PerformRoute(const RouteData& route_data)
{
	if (route_data.dst_id == LocalID) return LocalRelayID;

	power.routing();
	power.selection();
	return Strategy.Apply(*this, Algorithm.Route(*this, route_data), route_data);
}

Router::Router(sc_module_name, int32_t id, size_t relays, int32_t max_buffer_size,
	RoutingAlgorithm& alg, SelectionStrategy& sel) : LocalID(id),
	Relays("RouterRelays", relays + 1), LocalRelay(Relays[relays]),
	LocalRelayID(relays), Algorithm(alg), Strategy(sel), stats(id, Relays.size())
{
	SC_METHOD(Update);
	sensitive << reset << clock.pos();

	start_from_port = LocalRelayID;

	for (size_t i = 0; i < Relays.size(); i++)
		Relays[i].buffer.SetMaxBufferSize(max_buffer_size);
}