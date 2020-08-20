#include "GlobalStats.hpp"
#include <iostream>
#include <iomanip>



GlobalStats::GlobalStats(const NoC& network, const Configuration& config) : 
	Network(network), Config(config)
{
}

std::int32_t GlobalStats::GetMaxBufferStuckDelay() const
{
	double max_delay = 0.0;
	int i = -1, r;
	for (const auto& t : Network.Tiles)
	{
		i++;
		double min_pop_time = t.RouterDevice->stats.GetMinBufferPopOrEmptyTime();

		if (Config.SimulationTime() + Config.ResetTime() - min_pop_time > max_delay)
		{
			max_delay = Config.SimulationTime() + Config.ResetTime() - min_pop_time;
			r = i;
		}
	}
	return static_cast<std::int32_t>(max_delay);
}
double GlobalStats::GetAverageBufferLoad(std::int32_t channel) const
{
	double sum = 0.0;
	for (const auto& t : Network.Tiles)
		sum += t.RouterDevice->stats.GetAVGBufferLoad(channel, Config.ChannelsCount());
	return sum / Network.Tiles.size();
}
double GlobalStats::GetAverageBufferLoad() const
{
	double sum = 0.0;
	for (int i = 0; i < Config.ChannelsCount(); i++)
		sum += GetAverageBufferLoad(i);
	return sum / Config.ChannelsCount();
}
std::int32_t GlobalStats::getLastReceivedFlitTime() const
{
	double result = 0.0;
	for (const auto& t : Network.Tiles) if (t.RouterDevice->stats.getLastReceivedFlitTime() > result)
		result = t.RouterDevice->stats.getLastReceivedFlitTime();
	return static_cast<std::int32_t>(result);
}
double GlobalStats::getAverageDelay() const
{
	unsigned int total_packets = 0;
	double avg_delay = 0.0;

	for (const auto& t : Network.Tiles)
	{
		uint32_t received_packets = t.RouterDevice->stats.getReceivedPackets();
		if (received_packets)
		{
			avg_delay += received_packets * t.RouterDevice->stats.getAverageDelay();
			total_packets += received_packets;
		}
	}

	avg_delay /= (double)total_packets;

	return avg_delay;
}
double GlobalStats::getAverageDelay(int src_id, int dst_id) const
{
	return Network.Tiles[dst_id].RouterDevice->stats.getAverageDelay(src_id);
}
double GlobalStats::getMaxDelay() const
{
	double maxd = -1.0;

	for (std::int32_t y = 0; y < Network.Tiles.size(); y++)
	{
		double d = getMaxDelay(y);
		if (d > maxd) maxd = d;
	}

	return maxd;
}
double GlobalStats::getMaxDelay(std::int32_t node_id) const
{
	unsigned int received_packets = Network.Tiles[node_id].RouterDevice->stats.getReceivedPackets();
	if (received_packets) return Network.Tiles[node_id].RouterDevice->stats.getMaxDelay();
	else return -1.0;
}
double GlobalStats::getMaxDelay(int src_id, int dst_id) const
{
	return Network.Tiles[dst_id].RouterDevice->stats.getMaxDelay(src_id);
}
double GlobalStats::getAverageThroughput(int src_id, int dst_id)
{
	return Network.Tiles[dst_id].RouterDevice->stats.getAverageThroughput(src_id);
}
double GlobalStats::getAggregatedProduction() const
{
	std::int32_t total_cycles = Config.SimulationTime() - Config.StatsWarmUpTime();
	return static_cast<double>(GetTotalFlitsGenerated()) / static_cast<double>(total_cycles);
}
double GlobalStats::getAggregatedThroughput() const
{
	std::int32_t total_cycles = Config.SimulationTime() - Config.StatsWarmUpTime();
	return (double)getReceivedFlits() / (double)total_cycles;
}
double GlobalStats::getAggregatedAcceptance() const
{
	std::int32_t total_cycles = Config.SimulationTime() - Config.StatsWarmUpTime();
	return (double)getAcceptedFlits() / (double)total_cycles;
}
unsigned int GlobalStats::getReceivedPackets() const
{
	unsigned int n = 0;

	for (const auto& t : Network.Tiles)
		n += t.RouterDevice->stats.getReceivedPackets();

	return n;
}
unsigned int GlobalStats::getReceivedFlits() const
{
	uint32_t n = 0;
	for (const auto& t : Network.Tiles) n += t.RouterDevice->stats.getReceivedFlits();
	return n;
}
unsigned int GlobalStats::getAcceptedFlits() const
{
	uint32_t n = 0;
	for (const auto& t : Network.Tiles) n += t.RouterDevice->stats.GetAcceptedFlits();
	return n;
}
double GlobalStats::getThroughput() const
{
	return (double)getAggregatedThroughput() / (double)(Network.Tiles.size());
}
// Only accounting IP that received at least one flit
double GlobalStats::getActiveThroughput() const
{
	int total_cycles = Config.SimulationTime() - Config.StatsWarmUpTime();
	unsigned int n = 0;
	unsigned int trf = 0;
	unsigned int rf;

	for (const auto& t : Network.Tiles)
	{
		rf = t.RouterDevice->stats.getReceivedFlits();
		if (rf != 0) n++;
		trf += rf;
	}

	return (double)trf / (double)(total_cycles * n);
}
double GlobalStats::getDynamicPower() const
{
	double power = 0.0;
	for (const auto& t : Network.Tiles)
	{
		power += t.RouterDevice->power.getDynamicPower();
	}
	return power;
}
double GlobalStats::getStaticPower() const
{
	double power = 0.0;

	for (const auto& t : Network.Tiles)
	{
		power += t.RouterDevice->power.getStaticPower();
	}
	return power;
}
void GlobalStats::updatePowerBreakDown(std::map<std::string, double>& dst, PowerBreakdown* src) const
{
	for (int i = 0; i != src->size; i++)
		dst[src->breakdown[i].label] += src->breakdown[i].value;
}
double GlobalStats::getReceivedIdealFlitRatio() const
{
	int total_cycles;
	total_cycles = Config.SimulationTime() - Config.StatsWarmUpTime();
	double ratio;
	ratio = getReceivedFlits() / (Config.PacketInjectionRate() * (Config.MinPacketSize() +
		Config.MaxPacketSize()) / 2 * total_cycles * Network.Tiles.size());
	return ratio;
}
std::int32_t GlobalStats::GetTotalFlitsGenerated() const
{
	std::int32_t result = 0;
	for (const auto& t : Network.Tiles) result += t.ProcessorDevice->GetTotalFlitsGenerated();
	return result;
}
void GlobalStats::ShowBuffers(std::ostream& out) const
{
	out << "% Buffer statuses:\n";
	for (std::int32_t i = 0; i < Config.TopologyGraph().size(); i++)
	{
		const auto& node = Config.TopologyGraph()[i];
		const auto& relays = Network.Tiles[i].RouterDevice->Relays;
		const auto& stats = Network.Tiles[i].RouterDevice->stats;
		for (std::int32_t r = 0; r < relays.size(); r++)
		{
			out << '[';
			out << i << "<-(" << r << ")--";
			if (r < node.size()) out << node[r] << ']';
			else out << "L]";
			out << ": " << Config.SimulationTime() + Config.ResetTime() - static_cast<std::int32_t>(stats.GetBufferPopOrEmptyTime(r)) << " ";
			out << relays[r].buffer << '\n';
		}
	}
}

void GlobalStats::showPowerBreakDown(std::ostream& out)
{
	std::map<std::string, double> power_dynamic;
	std::map<std::string, double> power_static;

	for (const auto& t : Network.Tiles)
	{
		updatePowerBreakDown(power_dynamic, t.RouterDevice->power.getDynamicPowerBreakDown());
		updatePowerBreakDown(power_static, t.RouterDevice->power.getStaticPowerBreakDown());
	}
	printMap("power_dynamic", power_dynamic, out);
	printMap("power_static", power_static, out);
}
std::ostream& operator<<(std::ostream& out, const GlobalStats& gs)
{
	out << "% Last time flit received: " << gs.getLastReceivedFlitTime() << '\n';
	out << "% Max buffer stuck delay: " << gs.GetMaxBufferStuckDelay() << '\n';
	out << "% Total produced flits: " << gs.GetTotalFlitsGenerated() << '\n';
	out << "% Total accepted flits: " << gs.getAcceptedFlits() << '\n';
	out << "% Total received flits: " << gs.getReceivedFlits() << '\n';
	out << "% Network production (flits/cycle): " << gs.getAggregatedProduction() << '\n';
	out << "% Network acceptance (flits/cycle): " << gs.getAggregatedAcceptance() << '\n';
	out << "% Network throughput (flits/cycle): " << gs.getAggregatedThroughput() << '\n';
	out << "% Total received packets: " << gs.getReceivedPackets() << '\n';
	out << "% Global average delay (cycles): " << gs.getAverageDelay() << '\n';
	out << "% Max delay (cycles): " << gs.getMaxDelay() << '\n';
	out << "% Received/Ideal flits Ratio: " << gs.getReceivedIdealFlitRatio() << '\n';
	out << "% Average IP throughput (flits/cycle/IP): " << gs.getThroughput() << '\n';
	out << "% Average buffer utilization: " << gs.GetAverageBufferLoad() << '\n';
	if (gs.Config.ChannelsCount() > 1)
	{
		for (std::int32_t i = 0; i < gs.Config.ChannelsCount(); i++)
			out << "% Average channel " << i << " utilization: " << gs.GetAverageBufferLoad(i) << '\n';
	}
	out << "% Total energy (J): " << gs.getTotalPower() << '\n';
	out << "% Dynamic energy (J): " << gs.getDynamicPower() << '\n';
	out << "% Static energy (J): " << gs.getStaticPower() << '\n';

	if (gs.Config.ReportBuffers()) gs.ShowBuffers(out);
	return out;
}

