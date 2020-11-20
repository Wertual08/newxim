#include "PerFlitRouter.hpp"



void PerFlitRouter::TXProcess()
{
	for (std::int32_t j = 0; j < Relays.size(); j++)
	{
		std::int32_t in_port = (start_from_port + j) % Relays.size();
		Relay& rel = Relays[in_port];

		if (rel.FlitAvailable())
		{
			Flit flit = rel.Front();
			
			Connection dst = FindDestination(flit);

			if (!dst.valid()) continue;

			Route(in_port, dst);
		}
	} // for loop directions

	for (auto& relay : Relays) relay.Skip();
}

PerFlitRouter::PerFlitRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays) :
	Router(timer, id, relays)
{
}
