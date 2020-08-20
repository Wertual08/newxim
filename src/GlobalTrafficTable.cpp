#include "GlobalTrafficTable.hpp"
#include <fstream>
#include <cassert>



GlobalTrafficTable::GlobalTrafficTable(double reset_time, double simulation_time, double pir) :
	ResetTime(reset_time), SimulationTime(simulation_time), PIR(pir)
{
}

bool GlobalTrafficTable::load(const char* fname)
{
	// Open file
	std::ifstream fin(fname, std::ios::in);
	if (!fin)
		return false;

	// Initialize variables
	traffic_table.clear();

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
					else communication.pir = PIR;

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
					else communication.t_off = ResetTime + SimulationTime;

					// Custom Tperiod
					if (params >= 7 && t_period > 0) {
						assert(t_period > t_off);
						communication.t_period = t_period;
					}
					else communication.t_period = ResetTime + SimulationTime;

					// Add this communication to the vector of communications
					traffic_table.push_back(communication);
				}
			}
		}
	}

	return true;
}

double GlobalTrafficTable::getCumulativePirPor(int src_id, int ccycle, bool pir_not_por,
	std::vector<std::pair<int, double>>& dst_prob) const
{
	double cpirnpor = 0.0;

	dst_prob.clear();

	for (unsigned int i = 0; i < traffic_table.size(); i++) {
		Communication comm = traffic_table[i];
		if (comm.src == src_id) {
			int r_ccycle = ccycle % comm.t_period;
			if (r_ccycle > comm.t_on && r_ccycle < comm.t_off) {
				cpirnpor += pir_not_por ? comm.pir : comm.por;
				std::pair<int, double> dp(comm.dst, cpirnpor);
				dst_prob.push_back(dp);
			}
		}
	}

	return cpirnpor;
}

int GlobalTrafficTable::occurrencesAsSource(int src_id) const
{
	int count = 0;

	for (unsigned int i = 0; i < traffic_table.size(); i++)
		if (traffic_table[i].src == src_id)
			count++;

	return count;
}
