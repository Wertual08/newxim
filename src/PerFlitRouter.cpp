#include "PerFlitRouter.h"



void PerFlitRouter::TXProcess()
{
	for (int32_t j = 0; j < Relays.size(); j++)
	{
		int32_t i = (start_from_port + j) % Relays.size();
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
			int32_t res = PerformRoute(route_data);
			if (res < 0) continue;

			int32_t out_port = res;
			Relay& out_rel = Relays[out_port];

			bool buf_stat = out_rel.tx_buffer_full_status.read();

			if ((out_rel.tx_current_level == out_rel.tx_ack.read()) && !buf_stat)
			{
				rel.buffer.Pop();
				flit.hop_no++;
				out_rel.tx_flit.write(flit);
				out_rel.tx_current_level = 1 - out_rel.tx_current_level;
				out_rel.tx_req.write(out_rel.tx_current_level);

				/* Power & Stats ------------------------------------------------- */
				stats.UpdateBufferPopOrEmptyTime(i, sc_time_stamp().to_double() / GlobalParams::clock_period_ps);

				power.r2rLink();

				power.bufferRouterPop();
				power.crossBar();

				if (out_port == LocalRelayID)
				{
					power.networkInterface();
					stats.receivedFlit(sc_time_stamp().to_double() / GlobalParams::clock_period_ps, flit);
				}
				/* End Power & Stats ------------------------------------------------- */
				break;
			}
		}
		else stats.UpdateBufferPopOrEmptyTime(i, sc_time_stamp().to_double() / GlobalParams::clock_period_ps);
	} // for loop directions

}
void PerFlitRouter::RXProcess()
{
	// This process simply sees a flow of incoming flits. All arbitration
	// and wormhole related issues are addressed in the txProcess()
	for (size_t i = 0; i < Relays.size(); i++)
	{
		Relay& rel = Relays[i];
		// To accept a new flit, the following conditions must match:
		// 1) there is an incoming request
		// 2) there is a free slot in the input buffer of direction i

		if (rel.rx_req.read() == 1 - rel.rx_current_level)
		{
			Flit received_flit = rel.rx_flit.read();

			int vc = received_flit.vc_id;

			if (!rel.buffer.IsFull())
			{
				// Store the incoming flit in the circular buffer
				rel.buffer.Push(received_flit);

				power.bufferRouterPush();

				// Negate the old value for Alternating Bit Protocol (ABP)
				rel.rx_current_level = 1 - rel.rx_current_level;

				// if a new flit is injected from local PE
				if (received_flit.src_id == LocalID)
				{
					stats.AcceptFlit(sc_time_stamp().to_double() / GlobalParams::clock_period_ps);
					power.networkInterface();
				}
			}
			else  // buffer full
			{
				// should not happen with the new TBufferFullStatus control signals    
				// except for flit coming from local PE, which don't use it 
				assert(i == LocalRelayID);
			}

		}
		rel.rx_ack.write(rel.rx_current_level);
		// updates the mask of VCs to prevent incoming data on full buffers
		rel.rx_buffer_full_status.write(rel.buffer.IsFull());
	}
}

PerFlitRouter::PerFlitRouter(sc_module_name mname, int32_t id, size_t relays,
	int32_t max_buffer_size, RoutingAlgorithm& alg, SelectionStrategy& sel) :
	Router(mname, id, relays, max_buffer_size, alg, sel)
{
}
