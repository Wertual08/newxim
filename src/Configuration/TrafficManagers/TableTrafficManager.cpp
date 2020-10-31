#include "TableTrafficManager.hpp"
#include <fstream>
#include <cassert>



TableTrafficManager::TableTrafficManager(std::uint32_t seed, std::int32_t count, 
	const std::string& file, double default_pir, double total_global_time) :
	Random(seed), DestDistribution(0, count - 1), FireDistribution(0, 1)
{
	// Open file
	std::ifstream fin(file, std::ios::in);
	if (!fin) throw std::runtime_error("TableTrafficManager error: File [" + file + "] does not exsits.");

	// Initialize variables
	TrafficTable.clear();

	// Cycle reading file
	while (!fin.eof()) {
		char line[512];
		fin.getline(line, sizeof(line) - 1);

		if (line[0] != '\0') {
			if (line[0] != '%') {
				int src, dst;	// Mandatory
				double pir, por;
				int t_on, t_off, t_period;

				int params =
					sscanf(line, "%d %d %lf %lf %d %d %d", &src, &dst, &pir,
						&por, &t_on, &t_off, &t_period);
				if (params >= 2) {
					// Create a communication from the parameters read on the line
					Communication communication;

					// Mandatory fields
					communication.src = src;
					communication.dst = dst;

					// Custom PIR
					if (params >= 3 && pir >= 0 && pir <= 1) communication.pir = pir;
					else communication.pir = default_pir;

					// Custom POR
					if (params >= 4 && por >= 0 && por <= 1) communication.por = por;
					else communication.por = communication.pir;	// GlobalParams::probability_of_retransmission;

					  // Custom Ton
					if (params >= 5 && t_on >= 0) communication.t_on = t_on;
					else communication.t_on = 0;

					// Custom Toff
					if (params >= 6 && t_off >= 0)
					{
						assert(t_off > t_on);
						communication.t_off = t_off;
					}
					else communication.t_off = total_global_time;

					// Custom Tperiod
					if (params >= 7 && t_period > 0) {
						assert(t_period > t_off);
						communication.t_period = t_period;
					}
					else communication.t_period = total_global_time;

					// Add this communication to the vector of communications
					TrafficTable.push_back(communication);
				}
			}
		}
	}
}

bool TableTrafficManager::FirePacket(std::int32_t from, double time, bool retransmitting) const
{
	double threshold = 0.0;
	for (const Communication& comm : TrafficTable)
	{
		if (comm.src == from) 
		{
			int r_ccycle = static_cast<std::int32_t>(time) % comm.t_period;
			if (r_ccycle > comm.t_on && r_ccycle < comm.t_off)
				threshold += retransmitting ? comm.por : comm.pir;
		}
	}

	return (FireDistribution(Random) < threshold);
}
std::int32_t TableTrafficManager::FindDestination(std::int32_t from) const
{
	std::int32_t destination;
	while ((destination = DestDistribution(Random)) == from);
	return destination;
}
