#include "WormholeRouter.hpp"



void WormholeRouter::TXProcess()
{
	// 1st phase: Reservation
	for (std::int32_t j = 0; j < Relays.size(); j++)
	{
		std::int32_t i = (start_from_port + j) % Relays.size();
		Relay& rel = Relays[i];

		if (!rel.buffer.IsEmpty())
		{
			Flit flit = rel.buffer.Front();
			power.bufferRouterFront();

			if (flit.flit_type == FLIT_TYPE_HEAD)
			{
				// prepare data for routing
				RouteData route_data;
				route_data.hop_no = flit.hop_no;
				route_data.current_id = LocalID;
				route_data.src_id = flit.src_id;
				route_data.dst_id = flit.dst_id;
				route_data.dir_in = i;
				route_data.sequence_length = flit.sequence_length;

				// TODO: see PER POSTERI (adaptive routing should not recompute route if already reserved)
				std::int32_t out = PerformRoute(route_data);
				if (out < 0) continue;
				
				if (!reservation_table.Reserved(out))
					reservation_table.Reserve(i, out);
			}
		}
	}

	// 2nd phase: Forwarding
	for (std::int32_t i = 0; i < Relays.size(); i++)
	{
		Relay& rel = Relays[i];

		if (!rel.buffer.IsEmpty())
		{
			std::int32_t res = reservation_table.Reservation(i);
			if (res < 0) continue;

			Route(i, res);
		}
		else stats.UpdateBufferPopOrEmptyTime(i);
	} // for loop directions
}

WormholeRouter::WormholeRouter(const SimulationTimer& timer, std::int32_t id, size_t relays, std::int32_t max_buffer_size) : 
	Router(timer, id, relays, max_buffer_size), reservation_table(Relays.size())
{
}
