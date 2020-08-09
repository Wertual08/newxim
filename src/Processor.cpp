#include "Processor.hpp"



static int randInt(int min, int max)
{
	return min + (int)((double)(max - min + 1) * rand() / (RAND_MAX + 1.0));
}


void Processor::UpdateCurrentPacket()
{
	current_packet.src_id = local_id;

	// Random destination distribution
	current_packet.dst_id = Traffic.FindDestination(local_id);

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

void Processor::rxProcess()
{
	if (reset.read()) 
	{
		ack_rx.write(0);
		current_level_rx = 0;
	}
	else 
	{
		if (req_rx.read() == 1 - current_level_rx) 
		{
			Flit flit_tmp = flit_rx.read();
			current_level_rx = 1 - current_level_rx;	// Negate the old value for Alternating Bit Protocol (ABP)
		}
		ack_rx.write(current_level_rx);
	}
}
void Processor::txProcess()
{
	if (reset.read()) 
	{
		TotalFlitsGenerated = 0;
		req_tx.write(0);
		current_level_tx = 0;
		transmittedAtPreviousCycle = false;
	}
	else 
	{
		if (canShot())
		{
			PushPacket();
			if (Timer.StatisticsTime() < 0.0) TotalFlitsGenerated += FrontPacket().size;
			transmittedAtPreviousCycle = true;
		}
		else transmittedAtPreviousCycle = false;


		if (ack_tx.read() == current_level_tx) 
		{
			if (!PacketQueueEmpty()) 
			{
				Flit flit = nextFlit();	// Generate a new flit
				flit_tx->write(flit);	// Send the generated flit
				current_level_tx = 1 - current_level_tx;	// Negate the old value for Alternating Bit Protocol (ABP)
				req_tx.write(current_level_tx);
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

		shot = Traffic.FirePacket(local_id, threshold);
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

int32_t Processor::GetTotalFlitsGenerated() const
{
	return TotalFlitsGenerated;
}
