#include "WormholeSubnetworkRouter.hpp"



static bool VectorContains(const std::vector<std::int32_t>& vec, std::int32_t v)
{
	return std::find(vec.begin(), vec.end(), v) != vec.end();
}
static bool VectorContains(const std::vector<std::vector<std::int32_t>>& vec, std::int32_t v)
{
	for (const auto& sub_vec : vec)
	{
		if (std::find(sub_vec.begin(), sub_vec.end(), v) != sub_vec.end())
			return true;
	}
	return false;
}

void WormholeSubnetworkRouter::TXProcess()
{
	for (std::int32_t j = 0; j < Relays.size(); j++)
	{
		std::int32_t in_port = (start_from_port + j) % static_cast<std::int32_t>(Relays.size());
		Relay& rel = Relays[in_port];

		if (rel.FlitAvailable())
		{
			Flit flit = rel.Front();
			std::int32_t vc_in = flit.vc_id;
			flit.dir_in = in_port;
			power.bufferRouterFront();

			if (HasFlag(flit.flit_type, FlitType::Head))
			{
				std::int32_t out_port = PerformRoute(flit);
				if (out_port < 0) continue;

				// TODO: Maybe it should select one of the channels... (Apply selection strategy???)
				if (Relays[out_port].GetFreeSlots(flit.vc_id) < 1)
					out_port = SubnetworkTable[flit.dst_id][0];
				if (Relays[out_port].GetFreeSlots(flit.vc_id) < 1)
					continue;

				if (!reservation_table.Reserved(out_port, flit.vc_id))
					reservation_table.Reserve(in_port, vc_in, out_port, flit.vc_id);
			}
		}
	}

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
			if (Route(in_port, res, flit.vc_id) && HasFlag(flit.flit_type, FlitType::Tail))
				reservation_table.Release(in_port, vc_in);
		}
	}
}

WormholeSubnetworkRouter::WormholeSubnetworkRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays) :
	SubnetworkRouter(timer, id, relays), reservation_table(Relays.size())
{
}
