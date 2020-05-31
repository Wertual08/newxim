/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the processing element
 */

#include "Processor.h"

int Processor::randInt(int min, int max)
{
	return min + (int)((double)(max - min + 1) * rand() / (RAND_MAX + 1.0));
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
		Packet packet;

		if (canShot(packet))
		{
			if (sc_time_stamp().to_double() / GlobalParams::clock_period_ps - GlobalParams::reset_time > 
				GlobalParams::stats_warm_up_time) TotalFlitsGenerated += packet.size;
			packet_queue.push(packet);
			transmittedAtPreviousCycle = true;
		}
		else transmittedAtPreviousCycle = false;


		if (ack_tx.read() == current_level_tx) 
		{
			if (!packet_queue.empty()) 
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
	Packet packet = packet_queue.front();

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

	packet_queue.front().flit_left--;
	if (packet_queue.front().flit_left == 0)
		packet_queue.pop();

	return flit;
}

bool Processor::canShot(Packet& packet)
{
	if (never_transmit) return false;
	bool shot;
	double threshold;

	double now = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;

	if (GlobalParams::traffic_distribution != TRAFFIC_TABLE_BASED) {
		if (!transmittedAtPreviousCycle) threshold = GlobalParams::packet_injection_rate;
		else threshold = GlobalParams::probability_of_retransmission;

		shot = (((double)rand()) / RAND_MAX < threshold);
		if (shot)
		{
			if (GlobalParams::traffic_distribution == TRAFFIC_RANDOM) packet = trafficRandom();
			else
			{
				cout << "Invalid traffic distribution: " << GlobalParams::traffic_distribution << endl;
				exit(-1);
			}
		}
	}
	else {			// Table based communication traffic
		if (never_transmit) return false;

		bool use_pir = (transmittedAtPreviousCycle == false);
		std::vector<std::pair<int, double>> dst_prob;
		double threshold =
			traffic_table->getCumulativePirPor(local_id, (int)now, use_pir, dst_prob);

		double prob = (double)rand() / RAND_MAX;
		shot = (prob < threshold);
		if (shot) {
			for (unsigned int i = 0; i < dst_prob.size(); i++) {
				if (prob < dst_prob[i].second) {
					int vc = 0;
					packet.make(local_id, dst_prob[i].first, vc, now, getRandomSize());
					break;
				}
			}
		}
	}

	return shot;
}

Packet Processor::trafficLocal()
{
	Packet p;
	p.src_id = local_id;
	double rnd = rand() / (double)RAND_MAX;

	std::vector<int> dst_set;
	int i_rnd = rand() % dst_set.size();

	p.dst_id = dst_set[i_rnd];
	p.timestamp = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
	p.size = p.flit_left = getRandomSize();
	p.vc_id = 0;

	return p;
}

Packet Processor::trafficRandom()
{
	Packet p;
	p.src_id = local_id;
	double rnd = rand() / (double)RAND_MAX;
	double range_start = 0.0;

	// Random destination distribution
	while ((p.dst_id = randInt(0, MaxID)) == p.src_id);

	p.timestamp = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
	p.size = p.flit_left = getRandomSize();
	p.vc_id = 0;

	return p;
}

int Processor::getRandomSize()
{
	return randInt(GlobalParams::min_packet_size, GlobalParams::max_packet_size);
}

unsigned int Processor::getQueueSize() const
{
	return packet_queue.size();
}

int32_t Processor::GetTotalFlitsGenerated() const
{
	return TotalFlitsGenerated;
}

