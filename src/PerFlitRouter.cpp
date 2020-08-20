#include "PerFlitRouter.hpp"



void PerFlitRouter::TXProcess()
{
	for (std::int32_t j = 0; j < Relays.size(); j++)
	{
		std::int32_t i = (start_from_port + j) % Relays.size();
		Relay& rel = Relays[i];

		if (!rel.buffer.IsEmpty())
		{
			Flit flit = rel.buffer.Front();

			RouteData route_data;
			route_data.hop_no = flit.hop_no;
			route_data.current_id = LocalID;
			route_data.src_id = flit.src_id;
			route_data.dst_id = flit.dst_id;
			route_data.dir_in = i;
			route_data.sequence_length = flit.sequence_length;
			std::int32_t res = PerformRoute(route_data);
			if (res < 0) continue;

			Route(i, res);
		}
		else stats.UpdateBufferPopOrEmptyTime(i);
	} // for loop directions

}

PerFlitRouter::PerFlitRouter(const SimulationTimer& timer, std::int32_t id, size_t relays, std::int32_t max_buffer_size) :
	Router(timer, id, relays, max_buffer_size)
{
}
