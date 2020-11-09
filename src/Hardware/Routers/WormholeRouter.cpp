#include "WormholeRouter.hpp"



void WormholeRouter::TXProcess()
{
	// 1st phase: Reservation
	for (std::int32_t j = 0; j < Relays.size(); j++)
	{
		std::int32_t in_port = (start_from_port + j) % Relays.size();
		Relay& rel = Relays[in_port];

		if (rel.FlitAvailable())
		{
			Flit flit = rel.Front();
			std::int32_t vc_in = flit.vc_id;
			flit.dir_in = in_port;
			power.bufferRouterFront();

			if (HasFlag(flit.flit_type, FlitType::Head))
			{
				// TODO: see PER POSTERI (adaptive routing should not recompute route if already reserved)
				std::int32_t out = PerformRoute(flit);
				if (out < 0) continue;

				if (!reservation_table.Reserved(out, flit.vc_id))
					reservation_table.Reserve(in_port, vc_in, out, flit.vc_id);
			}
		}
	}

	// 2nd phase: Forwarding
	for (std::int32_t in_port = 0; in_port < Relays.size(); in_port++)
	{
		Relay& rel = Relays[in_port];

		if (rel.FlitAvailable())
		{
			Flit flit = rel.Front();
			std::int32_t vc_in = flit.vc_id;
			std::int32_t res = reservation_table.Reservation(in_port, vc_in);
			std::int32_t vc = reservation_table.ReservationVC(in_port, vc_in);
			if (res < 0) continue;

			flit.vc_id = vc;
			flit.dir_in = in_port;


			if (Route(in_port, res, vc) && HasFlag(flit.flit_type, FlitType::Tail))
				reservation_table.Release(in_port, vc_in);
		}
	} // for loop directions
}

WormholeRouter::WormholeRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays) :
	Router(timer, id, relays), reservation_table(Relays.size())
{
}
