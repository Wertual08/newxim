#include "Processor.hpp"
#include <iomanip>
#include <sstream>
#include <string>



static std::string GetProcessorName(std::size_t id)
{
	return (std::stringstream() << "Processor[" << std::setfill('0') << std::setw(3) << id << "]").str();
}
static std::size_t randInt(std::size_t min, std::size_t max)
{
	return min + rand() / (RAND_MAX + 1.0) * (max - min + 1);
}


Packet& Processor::GetQueueFront()
{
	if (Queue.UpdateRequired()) Queue.UpdateFrontPacket(
		local_id, 
		Traffic->FindDestination(local_id),
		randInt(MinPacketSize, MaxPacketSize)
	);
	return Queue.Front();
}

void Processor::ReceiveFlit(Flit flit)
{
	if (flit.dst_id != local_id) 
		throw std::runtime_error(
		"Processor received a flit that does not belong to it. LocalID[" + 
		std::to_string(local_id) + "] DestinationID[" + std::to_string(flit.dst_id) + "]");

	if (Timer.StatisticsTime() >= 0)
	{
		if (HasFlag(flit.flit_type, FlitType::Tail))
		{
			double delay = Timer.SystemTime() - flit.timestamp;
			TotalPacketsDelay += delay;
			if (delay > MaxPacketDelay) MaxPacketDelay = delay;

			TotalPacketsReceived++;
		}
		TotalFlitsReceived++;

		if (Timer.StatisticsTime() - flit.accept_timestamp > SimulationMaxTimeFlitInNetwork)
			SimulationMaxTimeFlitInNetwork = Timer.StatisticsTime() - flit.accept_timestamp;
		SimulationLastTimeFlitReceived = Timer.StatisticsTime();
	}
	TotalActualFlitsReceived++;
}
void Processor::SendFlit(Flit flit)
{
	if (Timer.StatisticsTime() >= 0) TotalFlitsSent++;
	TotalActualFlitsSent++;
}

Flit Processor::NextFlit()
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

	if (packet.size == packet.flit_left) flit.flit_type = flit.flit_type | FlitType::Head;
	if (packet.flit_left == 1) flit.flit_type = flit.flit_type | FlitType::Tail;
	if (packet.flit_left > 1 && packet.flit_left < packet.size) flit.flit_type = flit.flit_type | FlitType::Body;

	return flit;
}
void Processor::PopFlit()
{
	Packet& packet = GetQueueFront();
	packet.flit_left--;
	if (packet.flit_left == 0) Queue.Pop();
}

Processor::Processor(sc_module_name, const SimulationTimer& timer, std::int32_t id,
	std::int32_t min_packet_size, std::int32_t max_packet_size) :
	Timer(timer), local_id(id), MinPacketSize(min_packet_size),
	MaxPacketSize(max_packet_size), Traffic(nullptr)
{
	SC_METHOD(Update);
	sensitive << reset << clock.pos();
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
void Processor::SetFlitTracer(FlitTracer& tracer)
{
	Tracer = &tracer;
}

void Processor::Update()
{
	if (reset.read())
	{
		relay.Reset();

		TotalPacketsReceived = 0;
		TotalFlitsSent = 0;
		TotalFlitsReceived = 0;
		TotalActualFlitsSent = 0;
		TotalActualFlitsReceived = 0;
		
		TotalPacketsDelay = 0;
		MaxPacketDelay = 0;
		SimulationMaxTimeFlitInNetwork = 0;
		SimulationLastTimeFlitReceived = 0;
	}
	else
	{
		if (Timer.SimulationTime() < Timer.ProductionTime()) {
			if (Traffic->FirePacket(local_id, Timer.SystemTime())) {
				Queue.Push(Timer.SystemTime());
			}
		}

		TXProcess();
		RXProcess();
	}
}
void Processor::TXProcess()
{
	if (!Queue.Empty())
	{
		Flit flit = NextFlit();
		if (relay.CanSend(flit))
		{
			if (Tracer) Tracer->Register(flit);
			relay.Send(flit);
			PopFlit();
			SendFlit(flit);
		}
	}

}
void Processor::RXProcess()
{
	relay.Receive();
	Flit flit = relay.Pop();
	if (flit.valid()) {
		ReceiveFlit(flit);
	}
	relay.Update();
}

std::size_t Processor::FlitsSent() const
{
	return TotalFlitsSent;
}
std::size_t Processor::FlitsReceived() const
{
	return TotalFlitsReceived;
}
std::size_t Processor::FlitsProduced() const
{
	return TotalFlitsSent + Queue.Size() * (MaxPacketSize + MinPacketSize) / 2;
}
std::size_t Processor::ActualFlitsSent() const
{
	return TotalActualFlitsSent;
}
std::size_t Processor::ActualFlitsReceived() const
{
	return TotalActualFlitsReceived;
}

std::size_t Processor::PacketsReceived() const
{
	return TotalPacketsReceived;
}
double Processor::AverageDelay() const
{
	return TotalPacketsDelay / TotalPacketsReceived;
}
double Processor::MaxDelay() const
{
	return MaxPacketDelay;
}
double Processor::MaxTimeFlitInNetwork() const
{
	return SimulationMaxTimeFlitInNetwork;
}
double Processor::LastReceivedFlitTime() const
{
	return SimulationLastTimeFlitReceived;
}
