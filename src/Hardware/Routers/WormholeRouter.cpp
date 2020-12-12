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
			power.bufferRouterFront();

			if (HasFlag(flit.flit_type, FlitType::Head))
			{
				Connection src = { in_port, flit.vc_id };
				Connection dst = FindDestination(flit);
				if (!dst.valid()) continue;

				if (!reservation_table.Reserved(src, dst))
					reservation_table.Reserve(src, dst);
			}
		}
	}

	// 2nd phase: Forwarding
	for (std::int32_t in_port = 0; in_port < Relays.size(); in_port++)
	{
		Relay& rel = Relays[in_port];
		
		Flit flit = rel.Front();

		if (rel.FlitAvailable())
		{
			Flit flit = rel.Front();
			Connection src = { in_port, flit.vc_id };
			Connection dst = reservation_table[src];

			if (!dst.valid()) continue;

			if (Route(in_port, dst) && HasFlag(flit.flit_type, FlitType::Tail))
				reservation_table.Release(src);

			flit = rel.Front();
		}
	} // for loop directions

	for (auto& relay : Relays) relay.Skip();
}