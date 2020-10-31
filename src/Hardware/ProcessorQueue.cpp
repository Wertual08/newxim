#include "ProcessorQueue.hpp"



bool ProcessorQueue::UpdateRequired() const
{
	return update_required;
}
void ProcessorQueue::UpdateFrontPacket(std::int32_t src_id, std::int32_t dst_id, std::int32_t size)
{
	current_packet.src_id = src_id;

	// Random destination distribution
	current_packet.dst_id = dst_id;

	current_packet.timestamp = oldest_packet_time_stamp;
	current_packet.size = current_packet.flit_left = size;
	current_packet.vc_id = 0;
	update_required = false;
}

ProcessorQueue::ProcessorQueue() :
	packets_in_queue(0),
	oldest_packet_time_stamp(0.0),
	newest_packet_time_stamp(0.0),
	current_packet{},
	update_required(false)
{
}

void ProcessorQueue::Push(double time_stamp)
{
	if (!packets_in_queue)
	{
		oldest_packet_time_stamp = time_stamp;
		newest_packet_time_stamp = time_stamp;
		update_required = true;
	}
	else
	{
		oldest_packet_time_stamp = std::min(oldest_packet_time_stamp, time_stamp);
		newest_packet_time_stamp = std::max(newest_packet_time_stamp, time_stamp);
	}

	packets_in_queue++;
}
void ProcessorQueue::Pop()
{
	if (Empty()) throw std::runtime_error("ProcessorQueue error: No packets to pop.");
	oldest_packet_time_stamp += (newest_packet_time_stamp - oldest_packet_time_stamp) / packets_in_queue;
	packets_in_queue--;

	if (packets_in_queue) update_required = true;
}
Packet& ProcessorQueue::Front()
{
	if (update_required || Empty()) throw std::runtime_error("ProcessorQueue error: Front packet deprevated.");
	return current_packet;
}
bool ProcessorQueue::Empty() const
{
	return !packets_in_queue;
}
std::size_t ProcessorQueue::Size() const
{
	return packets_in_queue;
}
