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
			flit.dir_in = in_port;
			std::int32_t res = PerformRoute(flit);
			if (res < 0) continue;

			Route(in_port, res, flit.vc_id);
		}
	} // for loop directions

}

PerFlitRouter::PerFlitRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays) :
	Router(timer, id, relays)
{
}
