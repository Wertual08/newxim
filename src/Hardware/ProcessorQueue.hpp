#pragma once
#include "DataStructs.hpp"



class ProcessorQueue
{
private:
	std::size_t packets_in_queue;
	double oldest_packet_time_stamp;
	double newest_packet_time_stamp;
	Packet current_packet;
	bool update_required;

public:
	ProcessorQueue();

	bool UpdateRequired() const;
	void UpdateFrontPacket(std::int32_t src_id, std::int32_t dst_id, std::int32_t size);

	void Push(double time_stamp);
	void Pop();
	Packet& Front();
	bool Empty() const;
	std::size_t Size() const;
};