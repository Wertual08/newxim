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


Processor::Processor(sc_module_name, const SimulationTimer& timer, std::int32_t id, bool table_based, double packet_injection_rate,
	double probability_of_retransmission, std::int32_t min_packet_size, std::int32_t max_packet_size, std::int32_t max_id) :
	Timer(timer), local_id(id), TableBased(table_based), PacketInjectionRate(packet_injection_rate),
	ProbabilityOfRetransmission(probability_of_retransmission), MinPacketSize(min_packet_size),
	MaxPacketSize(max_packet_size), MaxID(max_id), Traffic(nullptr)
{
	SC_METHOD(rxProcess);
	sensitive << reset << clock.pos();
	SC_METHOD(txProcess);
	sensitive << reset << clock.pos();
}

void Processor::UpdateCurrentPacket()
{
	current_packet.src_id = local_id;

	// Random destination distribution
	current_packet.dst_id = Traffic->FindDestination(local_id);

	current_packet.timestamp = oldest_packet_time_stamp;
	current_packet.size = current_packet.flit_left = getRandomSize();
	current_packet.vc_id = 0;
}
void Processor::PushPacket()
{
	double time_stamp = Timer.SystemTime();
	if (!packets_in_queue)
	{
		oldest_packet_time_stamp = time_stamp;
		newest_packet_time_stamp = time_stamp;
		UpdateCurrentPacket();
	}
	else
	{
		oldest_packet_time_stamp = std::min(oldest_packet_time_stamp, time_stamp);
		newest_packet_time_stamp = std::max(newest_packet_time_stamp, time_stamp);
	}

	packets_in_queue++;
}
void Processor::PopPacket()
{
	oldest_packet_time_stamp += (newest_packet_time_stamp - oldest_packet_time_stamp) / packets_in_queue;
	packets_in_queue--;

	if (packets_in_queue) UpdateCurrentPacket();
}
Packet& Processor::FrontPacket()
{
	return current_packet;
}
bool Processor::PacketQueueEmpty() const
{
	return !packets_in_queue;
}
size_t Processor::PacketQueueSize() const
{
	return packets_in_queue;
}

Processor::Processor(const SimulationTimer& timer, std::int32_t id, bool table_based, 
	double packet_injection_rate, double probability_of_retransmission, 
	std::int32_t min_packet_size, std::int32_t max_packet_size, std::int32_t max_id) :
	Processor(GetProcessorName(id).c_str(), timer, id, table_based,
		packet_injection_rate, probability_of_retransmission,
		min_packet_size, max_packet_size, max_id)
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
	}
	else 
	{
		if (relay.rx_req.read() == 1 - current_level_rx)
		{
			Flit flit_tmp = relay.rx_flit.read();
			current_level_rx = 1 - current_level_rx;	// Negate the old value for Alternating Bit Protocol (ABP)
		}
		relay.rx_ack.write(current_level_rx);
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
		if (canShot())
		{
			PushPacket();
			if (Timer.StatisticsTime() > 0.0) TotalFlitsGenerated += FrontPacket().size;
			transmittedAtPreviousCycle = true;
		}
		else transmittedAtPreviousCycle = false;


		if (relay.tx_ack.read() == current_level_tx)
		{
			if (!PacketQueueEmpty()) 
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
	Flit flit;
	Packet packet = FrontPacket();

	flit.src_id = packet.src_id;
	flit.dst_id = packet.dst_id;
	flit.vc_id = packet.vc_id;
	flit.timestamp = packet.timestamp;
	flit.sequence_no = packet.size - packet.flit_left;
	flit.sequence_length = packet.size;
	flit.hop_no = 0;
	//  flit.payload     = DEFAULT_PAYLOAD;

	if (packet.size == packet.flit_left)
		flit.flit_type = FLIT_TYPE_HEAD;
	else if (packet.flit_left == 1)
		flit.flit_type = FLIT_TYPE_TAIL;
	else flit.flit_type = FLIT_TYPE_BODY;

	FrontPacket().flit_left--;
	if (FrontPacket().flit_left == 0)
		PopPacket();

	return flit;
}

bool Processor::canShot()
{
	if (never_transmit) return false;
	bool shot;

	double now = Timer.SystemTime();

	if (TableBased) 
	{
		// Table based communication traffic
		if (never_transmit) return false;

		bool use_pir = !transmittedAtPreviousCycle;
		std::vector<std::pair<int, double>> dst_prob;
		double threshold = traffic_table->getCumulativePirPor(local_id, (int)now, use_pir, dst_prob);

		double prob = (double)rand() / RAND_MAX;
		shot = (prob < threshold);
	}
	else 
	{
		double threshold;
		if (!transmittedAtPreviousCycle) threshold = PacketInjectionRate;
		else threshold = ProbabilityOfRetransmission;

		shot = Traffic->FirePacket(local_id, threshold);
	}

	return shot;
}

int Processor::getRandomSize()
{
	return randInt(MinPacketSize, MaxPacketSize);
}

unsigned int Processor::getQueueSize() const
{
	return PacketQueueSize();
}

std::int32_t Processor::GetTotalFlitsGenerated() const
{
	return TotalFlitsGenerated;
}
