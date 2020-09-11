#include "Processor.hpp"
#include <iomanip>
#include <sstream>
#include <string>



static std::string GetProcessorName(std::int32_t id)
{
	return (std::stringstream() << "Processor[" << std::setfill('0') << std::setw(3) << id << "]").str();
}
static std::int32_t randInt(std::int32_t min, std::int32_t max)
{
	return min + (int)((double)(max - min + 1) * rand() / (RAND_MAX + 1.0));
}


Packet& Processor::GetQueueFront()
{
	if (Queue.UpdateRequired()) Queue.UpdateFrontPacket(local_id, Traffic->FindDestination(local_id), getRandomSize());
	return Queue.Front();
}

void Processor::ReceiveFlit(Flit flit)
{
	if (flit.dst_id != local_id) throw std::runtime_error(
		"Processor received a flit that does not belong to it. LocalID[" + 
		std::to_string(local_id) + "] DestinationID[" + std::to_string(flit.dst_id) + "]");
}

Processor::Processor(sc_module_name, const SimulationTimer& timer, std::int32_t id,
	std::int32_t min_packet_size, std::int32_t max_packet_size) :
	Timer(timer), local_id(id), MinPacketSize(min_packet_size),
	MaxPacketSize(max_packet_size), Traffic(nullptr)
{
	SC_METHOD(rxProcess);
	sensitive << reset << clock.pos();
	SC_METHOD(txProcess);
	sensitive << reset << clock.pos();
	relay.buffer.SetMaxBufferSize(128);
}

Processor::Processor(const SimulationTimer& timer, std::int32_t id, 
	std::int32_t min_packet_size, std::int32_t max_packet_size) :
	Processor(GetProcessorName(id).c_str(), timer, id,
		min_packet_size, max_packet_size)
{
}
void Processor::SetTrafficManager(const TrafficManager& traffic)
{
	Traffic = &traffic;
}

void Processor::rxProcess()
{
	if (reset.read()) 
	{
		relay.rx_ack.write(0);
		current_level_rx = 0;
		relay.free_slots.write(relay.buffer.GetCurrentFreeSlots());
	}
	else 
	{
		if (relay.rx_req.read() == 1 - current_level_rx)
		{
			Flit flit = relay.rx_flit.read();
			current_level_rx = 1 - current_level_rx;	// Negate the old value for Alternating Bit Protocol (ABP)

			ReceiveFlit(flit);
		}
		relay.rx_ack.write(current_level_rx);
		relay.free_slots.write(relay.buffer.GetCurrentFreeSlots());
	}
}
void Processor::txProcess()
{
	if (reset.read()) 
	{
		TotalFlitsGenerated = 0;
		relay.tx_req.write(0);
		current_level_tx = 0;
		transmittedAtPreviousCycle = false;
	}
	else 
	{
		if (Traffic->FirePacket(local_id, Timer.SystemTime(), transmittedAtPreviousCycle))
		{
			Queue.Push(Timer.SystemTime());
			if (Timer.StatisticsTime() > 0.0) TotalFlitsGenerated += GetQueueFront().size;
			transmittedAtPreviousCycle = true;
		}
		else transmittedAtPreviousCycle = false;


		if (relay.tx_ack.read() == current_level_tx)
		{
			if (!Queue.Empty()) 
			{
				Flit flit = nextFlit();	// Generate a new flit
				relay.tx_flit->write(flit);	// Send the generated flit
				current_level_tx = 1 - current_level_tx;	// Negate the old value for Alternating Bit Protocol (ABP)
				relay.tx_req.write(current_level_tx);
			}
		}
	}
}

Flit Processor::nextFlit()
{
	Packet& packet = GetQueueFront();

	Flit flit;
	flit.src_id = packet.src_id;
	flit.dst_id = packet.dst_id;
	flit.vc_id = packet.vc_id;
	flit.timestamp = packet.timestamp;
	flit.accept_timestamp = Timer.StatisticsTime();
	flit.sequence_no = packet.size - packet.flit_left;
	flit.sequence_length = packet.size;
	flit.hop_no = 0;
	//  flit.payload     = DEFAULT_PAYLOAD;

	if (packet.size == packet.flit_left)
		flit.flit_type = FLIT_TYPE_HEAD;
	else if (packet.flit_left == 1)
		flit.flit_type = FLIT_TYPE_TAIL;
	else flit.flit_type = FLIT_TYPE_BODY;

	packet.flit_left--;
	if (packet.flit_left == 0)
		Queue.Pop();

	return flit;
}

int Processor::getRandomSize()
{
	return randInt(MinPacketSize, MaxPacketSize) / (rand() % 2 + 1);
}

std::int32_t Processor::GetTotalFlitsGenerated() const
{
	return TotalFlitsGenerated;
}
