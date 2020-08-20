#include "TreeBasedRerouteRouter.hpp"



static bool VectorContains(const std::vector<std::int32_t>& vec, std::int32_t v)
{
	return std::find(vec.begin(), vec.end(), v) != vec.end();
}

void TreeBasedRerouteRouter::TXProcess()
{
	for (std::int32_t j = 0; j < Relays.size(); j++)
	{
		std::int32_t in_port = (start_from_port + j) % static_cast<std::int32_t>(Relays.size());
		Relay& in_relay = Relays[in_port];

		if (!in_relay.buffer.IsEmpty())
		{
			Flit flit = in_relay.buffer.Front();

			RouteData route_data;
			route_data.hop_no = flit.hop_no;
			route_data.current_id = LocalID;
			route_data.src_id = flit.src_id;
			route_data.dst_id = flit.dst_id;
			route_data.dir_in = in_port;
			route_data.sequence_length = flit.sequence_length;
			std::int32_t out_port = PerformRoute(route_data);
			if (out_port < 0) continue;

			if (Relays[out_port].rx_buffer_full_status.read())
				out_port = SubTreeTable[flit.dst_id];

			Route(in_port, out_port);
		}
		else stats.UpdateBufferPopOrEmptyTime(in_port);
	} // for loop directions

}

TreeBasedRerouteRouter::TreeBasedRerouteRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays, std::int32_t max_buffer_size) :
	Router(timer, id, relays, max_buffer_size)
{
}
void TreeBasedRerouteRouter::SetupSubTreeTable(const std::vector<std::int32_t>& table)
{
	SubTreeTable = table;
}
