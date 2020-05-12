/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementaton of the global statistics
 */

#include "GlobalStats.h"
#include <iostream>
#include <iomanip>



GlobalStats::GlobalStats(const NoC& _noc, int32_t channels_count) : Network(_noc), ChannelsCount(channels_count)
{
}

double GlobalStats::GetMaxBufferStuckDelay() const
{
	double max_delay = 0.0;
	for (auto t : Network.Tiles)
	{
		double min_pop_time = t->RouterDevice.stats.GetMinBufferPopOrEmptyTime();

		if (GlobalParams::simulation_time + GlobalParams::reset_time - min_pop_time > max_delay)
			max_delay = GlobalParams::simulation_time + GlobalParams::reset_time - min_pop_time;
	}
	return max_delay;
}

double GlobalStats::GetAverageBufferLoad(int32_t channel) const
{
	double sum = 0.0;
	for (auto t : Network.Tiles)
		sum += t->RouterDevice.stats.GetAVGBufferLoad(channel, ChannelsCount);
	return sum / Network.Tiles.size();
}
double GlobalStats::GetAverageBufferLoad() const
{
	double sum = 0.0;
	for (int i = 0; i < ChannelsCount; i++)
		sum += GetAverageBufferLoad(i);
	return sum / ChannelsCount;
}

double GlobalStats::getLastReceivedFlitTime()
{
	double result = 0.0;
	for (auto t : Network.Tiles) if (t->RouterDevice.stats.getLastReceivedFlitTime() > result)
		result = t->RouterDevice.stats.getLastReceivedFlitTime();
	return result;
}

double GlobalStats::getAverageDelay()
{
	unsigned int total_packets = 0;
	double avg_delay = 0.0;

	for (auto t : Network.Tiles)
	{
		uint32_t received_packets = t->RouterDevice.stats.getReceivedPackets();
		if (received_packets)
		{
			avg_delay += received_packets * t->RouterDevice.stats.getAverageDelay();
			total_packets += received_packets;
		}
	}

	avg_delay /= (double)total_packets;

	return avg_delay;
}



double GlobalStats::getAverageDelay(int src_id, int dst_id)
{
	return Network.Tiles[dst_id]->RouterDevice.stats.getAverageDelay(src_id);
}

double GlobalStats::getMaxDelay()
{
	double maxd = -1.0;

	for (int32_t y = 0; y < Network.Tiles.size(); y++)
	{
		double d = getMaxDelay(y);
		if (d > maxd) maxd = d;
	}

	return maxd;
}

double GlobalStats::getMaxDelay(int32_t node_id)
{
	unsigned int received_packets = Network.Tiles[node_id]->RouterDevice.stats.getReceivedPackets();
	if (received_packets) return Network.Tiles[node_id]->RouterDevice.stats.getMaxDelay();
	else return -1.0;
}

double GlobalStats::getMaxDelay(int src_id, int dst_id)
{
	return Network.Tiles[dst_id]->RouterDevice.stats.getMaxDelay(src_id);
}

double GlobalStats::getAverageThroughput(int src_id, int dst_id)
{
	return Network.Tiles[dst_id]->RouterDevice.stats.getAverageThroughput(src_id);
}

/*
double GlobalStats::getAverageThroughput()
{
	unsigned int total_comms = 0;
	double avg_throughput = 0.0;

	for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++) {
		unsigned int ncomms =
		Network.t[x][y]->RouterDevice.stats.getTotalCommunications();

		if (ncomms) {
		avg_throughput +=
			ncomms * Network.t[x][y]->RouterDevice.stats.getAverageThroughput();
		total_comms += ncomms;
		}
	}

	avg_throughput /= (double) total_comms;

	return avg_throughput;
}
*/

double GlobalStats::getAggregatedThroughput() const
{
	int total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;
	return (double)getReceivedFlits() / (double)(total_cycles);
}
double GlobalStats::getAggregatedAcceptance() const
{
	int total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;
	return (double)getAcceptedFlits() / (double)(total_cycles);
}

unsigned int GlobalStats::getReceivedPackets()
{
	unsigned int n = 0;

	for (auto t : Network.Tiles)
		n += t->RouterDevice.stats.getReceivedPackets();

	return n;
}

unsigned int GlobalStats::getReceivedFlits() const
{
	uint32_t n = 0;
	for (auto t : Network.Tiles) n += t->RouterDevice.stats.getReceivedFlits();
	return n;
}
unsigned int GlobalStats::getAcceptedFlits() const
{
	uint32_t n = 0;
	for (auto t : Network.Tiles) n += t->RouterDevice.stats.GetAcceptedFlits();
	return n;
}

double GlobalStats::getThroughput()
{
	return (double)getAggregatedThroughput() / (double)(Network.Tiles.size());
}

// Only accounting IP that received at least one flit
double GlobalStats::getActiveThroughput()
{
	int total_cycles =
		GlobalParams::simulation_time -
		GlobalParams::stats_warm_up_time;
	unsigned int n = 0;
	unsigned int trf = 0;
	unsigned int rf;

	for (auto t : Network.Tiles)
	{
		rf = t->RouterDevice.stats.getReceivedFlits();
		if (rf != 0) n++;
		trf += rf;
	}

	return (double)trf / (double)(total_cycles * n);

}

double GlobalStats::getDynamicPower()
{
	double power = 0.0;
	for (auto t : Network.Tiles)
	{
		power += t->RouterDevice.power.getDynamicPower();
	}
	return power;
}

double GlobalStats::getStaticPower()
{
	double power = 0.0;

	for (auto t : Network.Tiles)
	{
		power += t->RouterDevice.power.getStaticPower();
	}
	return power;
}

void GlobalStats::showStats(std::ostream& out)
{
	out << "% Total received packets: " << getReceivedPackets() << '\n';
	out << "% Total received flits: " << getReceivedFlits() << '\n';
	out << "% Total accepted flits: " << getAcceptedFlits() << '\n';
	out << "% Received/Ideal flits Ratio: " << getReceivedIdealFlitRatio() << '\n';
	out << "% Last time flit received: " << static_cast<int32_t>(getLastReceivedFlitTime()) << '\n';
	out << "% Max buffer stuck delay: " << static_cast<int32_t>(GetMaxBufferStuckDelay()) << '\n';
	out << "% Global average delay (cycles): " << getAverageDelay() << '\n';
	out << "% Max delay (cycles): " << getMaxDelay() << '\n';
	out << "% Network throughput (flits/cycle): " << getAggregatedThroughput() << '\n';
	out << "% Network acceptance (flits/cycle): " << getAggregatedAcceptance() << '\n';
	out << "% Average IP throughput (flits/cycle/IP): " << getThroughput() << '\n';
	out << "% Average buffer utilization: " << GetAverageBufferLoad() << '\n';
	if (ChannelsCount > 1)
	{
		for (int32_t i = 0; i < ChannelsCount; i++)
		{
			out << "% Average channel " << i << " utilization: " << GetAverageBufferLoad(i) << '\n';
		}
	}
	out << "% Total energy (J): " << getTotalPower() << '\n';
	out << "% Dynamic energy (J): " << getDynamicPower() << '\n';
	out << "% Static energy (J): " << getStaticPower() << '\n';
}

void GlobalStats::updatePowerBreakDown(std::map<std::string, double>& dst, PowerBreakdown* src)
{
	for (int i = 0; i != src->size; i++)
		dst[src->breakdown[i].label] += src->breakdown[i].value;
}

void GlobalStats::showPowerManagerStats(std::ostream& out)
{
	std::streamsize p = out.precision();
	int total_cycles = sc_time_stamp().to_double() / GlobalParams::clock_period_ps - GlobalParams::reset_time;

	out.precision(4);

	out.unsetf(std::ios::fixed);

	out.precision(p);
}

void GlobalStats::showPowerBreakDown(std::ostream& out)
{
	std::map<std::string, double> power_dynamic;
	std::map<std::string, double> power_static;

	for (auto t : Network.Tiles)
	{
		updatePowerBreakDown(power_dynamic, t->RouterDevice.power.getDynamicPowerBreakDown());
		updatePowerBreakDown(power_static, t->RouterDevice.power.getStaticPowerBreakDown());
	}
	printMap("power_dynamic", power_dynamic, out);
	printMap("power_static", power_static, out);
}

double GlobalStats::getReceivedIdealFlitRatio()
{
	int total_cycles;
	total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;
	double ratio;
	ratio = getReceivedFlits() / (GlobalParams::packet_injection_rate * (GlobalParams::min_packet_size +
		GlobalParams::max_packet_size) / 2 * total_cycles * Network.Tiles.size());
	return ratio;
}
