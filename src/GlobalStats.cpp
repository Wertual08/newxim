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



GlobalStats::GlobalStats(const NoC& _noc) : Network(_noc)
{
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
	return Network.SearchNode(dst_id).RouterDevice.stats.getAverageDelay(src_id);
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
	return Network.SearchNode(dst_id).RouterDevice.stats.getMaxDelay(src_id);
}

double GlobalStats::getAverageThroughput(int src_id, int dst_id)
{
	return Network.SearchNode(dst_id).RouterDevice.stats.getAverageThroughput(src_id);
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

double GlobalStats::getAggregatedThroughput()
{
	int total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;

	return (double)getReceivedFlits() / (double)(total_cycles);
}

unsigned int GlobalStats::getReceivedPackets()
{
	unsigned int n = 0;

	for (auto t : Network.Tiles)
		n += t->RouterDevice.stats.getReceivedPackets();

	return n;
}

unsigned int GlobalStats::getReceivedFlits()
{
	unsigned int n = 0;
	for (auto t : Network.Tiles) n += t->RouterDevice.stats.getReceivedFlits();

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

void GlobalStats::showStats(std::ostream& out, bool detailed)
{
	//if (detailed)
	//{
	//	assert(GlobalParams::topology == TOPOLOGY_MESH);
	//	out << endl << "detailed = [" << endl;

	//	//for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	//	//	for (int x = 0; x < GlobalParams::mesh_dim_x; x++)
	//	//		Network.t[x][y]->RouterDevice.stats.showStats(y * GlobalParams::mesh_dim_x + x, out, true);
	//	for (int32_t i = 0; i < Network.Tiles.size(); i++)
	//	{
	//		Network.Tiles[i]->RouterDevice.stats.showStats(i, out, true);
	//	}
	//	out << "];" << endl;

	//	// show MaxDelay matrix
	//	vector < vector < double > > md_mtx = getMaxDelayMtx();

	//	out << endl << "max_delay = [" << endl;
	//	for (unsigned int y = 0; y < md_mtx.size(); y++)
	//	{
	//		out << "   ";
	//		for (unsigned int x = 0; x < md_mtx[y].size(); x++)
	//			out << setw(6) << md_mtx[y][x];
	//		out << endl;
	//	}
	//	out << "];" << endl;

	//	// show RoutedFlits matrix
	//	vector<vector<unsigned long>> rf_mtx = getRoutedFlitsMtx();

	//	out << endl << "routed_flits = [" << endl;
	//	for (unsigned int y = 0; y < rf_mtx.size(); y++)
	//	{
	//		out << "   ";
	//		for (unsigned int x = 0; x < rf_mtx[y].size(); x++)
	//			out << setw(10) << rf_mtx[y][x];
	//		out << endl;
	//	}
	//	out << "];" << endl;

	//	showPowerBreakDown(out);
	//	showPowerManagerStats(out);
	//}

	//int total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;
	out << "% Total received packets: " << getReceivedPackets() << endl;
	out << "% Total received flits: " << getReceivedFlits() << endl;
	out << "% Received/Ideal flits Ratio: " << getReceivedIdealFlitRatio() << endl;
	out << "% Global average delay (cycles): " << getAverageDelay() << endl;
	out << "% Max delay (cycles): " << getMaxDelay() << endl;
	out << "% Network throughput (flits/cycle): " << getAggregatedThroughput() << endl;
	out << "% Average IP throughput (flits/cycle/IP): " << getThroughput() << endl;
	out << "% Total energy (J): " << getTotalPower() << endl;
	out << "% Dynamic energy (J): " << getDynamicPower() << endl;
	out << "% Static energy (J): " << getStaticPower() << endl;

	if (GlobalParams::show_buffer_stats)
		showBufferStats(out);

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

void GlobalStats::showBufferStats(std::ostream& out)
{
	out << "Router id\tBuffer N\t\tBuffer E\t\tBuffer S\t\tBuffer W\t\tBuffer L" << endl;
	out << "         \tMean\tMax\tMean\tMax\tMean\tMax\tMean\tMax\tMean\tMax" << endl;

	for (auto t : Network.Tiles)
	{
		out << t->RouterDevice.local_id;
		t->RouterDevice.ShowBuffersStats(out);
		out << endl;
	}
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
