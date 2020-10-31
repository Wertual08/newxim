#include "WormholeFixedSubnetworkRouter.hpp"



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

void WormholeFixedSubnetworkRouter::TXProcess()
{
	for (std::int32_t j = 0; j < Relays.size(); j++)
	{
		std::int32_t in_port = (start_from_port + j) % static_cast<std::int32_t>(Relays.size());
		Relay& rel = Relays[in_port];

		if (!rel.buffer.IsEmpty())
		{
			Flit flit = rel.buffer.Front();
			power.bufferRouterFront();

			if (flit.flit_type == FLIT_TYPE_HEAD)
			{
				RouteData route_data;
				route_data.hop_no = flit.hop_no;
				route_data.current_id = LocalID;
				route_data.src_id = flit.src_id;
				route_data.dst_id = flit.dst_id;
				route_data.dir_in = in_port;
				route_data.sequence_length = flit.sequence_length;

				std::int32_t out_port = PerformRoute(route_data);
				if (out_port < 0) continue;

				// TODO: Maybe it should select one of the channels... (Apply selection strategy???)
				if (Relays[out_port].free_slots_neighbor.read() < 1 ||
					VectorContains(SubnetworkTable, in_port))
					out_port = SubnetworkTable[flit.dst_id][0];
				if (Relays[out_port].free_slots_neighbor.read() < 1)
					continue;

				if (!reservation_table.Reserved(out_port))
					reservation_table.Reserve(in_port, out_port);
			}
		}
	}

	for (std::int32_t i = 0; i < Relays.size(); i++)
	{
		Relay& rel = Relays[i];

		if (!rel.buffer.IsEmpty())
		{
			std::int32_t res = reservation_table.Reservation(i);
			if (res < 0) continue;

			Flit flit = rel.buffer.Front();
			if (Route(i, res) && flit.flit_type == FLIT_TYPE_TAIL)
				reservation_table.Release(i);
		}
		else stats.UpdateBufferPopOrEmptyTime(i);
	}
}

WormholeFixedSubnetworkRouter::WormholeFixedSubnetworkRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays, std::int32_t max_buffer_size) :
	SubnetworkRouter(timer, id, relays, max_buffer_size), reservation_table(Relays.size())
{
}
