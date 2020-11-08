#include "GlobalStats.hpp"
#include <iostream>
#include <iomanip>



GlobalStats::GlobalStats(const NoC& network, const Configuration& config) : 
	Network(network), Config(config)
{
}

std::size_t GlobalStats::GetActualFlitsReceived() const
{
	std::size_t n = 0;
	for (const auto& t : Network.Tiles) n += t.ProcessorDevice->ActualFlitsReceived();
	return n;
}
std::size_t GlobalStats::GetActualFlitsAccepted() const
{
	std::size_t n = 0;
	for (const auto& t : Network.Tiles) n += t.ProcessorDevice->ActualFlitsSent();
	return n;
}
std::size_t GlobalStats::GetFlitsInBuffers() const
{
	std::size_t count = 0;
	for (const auto& t : Network.Tiles)
		count += t.RouterDevice->TotalBufferedFlits();
	return count;
}
std::size_t GlobalStats::GetFlitsInTransmission() const
{
	std::size_t count = 0;
	for (const auto& t : Network.Tiles)
	{
		for (std::size_t r = 0; r < t.RouterDevice->Size(); r++)
			if ((*t.RouterDevice)[r].CanReceive()) count++;

		if ((*t.ProcessorDevice).relay.CanReceive()) count++;
	}
	return count;
}


std::size_t GlobalStats::GetFlitsProduced() const
{
	std::size_t result = 0;
	for (const auto& t : Network.Tiles) result += t.ProcessorDevice->FlitsProduced();
	return result;
}
std::size_t GlobalStats::GetFlitsAccepted() const
{
	std::size_t n = 0;
	for (const auto& t : Network.Tiles) n += t.ProcessorDevice->FlitsSent();
	return n;
}
std::size_t GlobalStats::GetFlitsReceived() const
{
	std::size_t n = 0;
	for (const auto& t : Network.Tiles) n += t.ProcessorDevice->FlitsReceived();
	return n;
}
double GlobalStats::GetProduction() const
{
	std::size_t total_cycles = Config.SimulationTime() - Config.StatsWarmUpTime();
	return static_cast<double>(GetFlitsProduced()) / static_cast<double>(total_cycles);
}
double GlobalStats::GetThroughput() const
{
	std::size_t total_cycles = Config.SimulationTime() - Config.StatsWarmUpTime();
	return static_cast<double>(GetFlitsReceived()) / static_cast<double>(total_cycles);
}
double GlobalStats::GetIPThroughput() const
{
	return GetThroughput() / static_cast<double>(Network.Tiles.size());
}
double GlobalStats::GetAcceptance() const
{
	std::size_t total_cycles = Config.SimulationTime() - Config.StatsWarmUpTime();
	return static_cast<double>(GetFlitsAccepted()) / static_cast<double>(total_cycles);
}

std::size_t GlobalStats::GetLastReceivedFlitTime() const
{
	double result = 0.0;
	for (const auto& t : Network.Tiles)
		if (t.ProcessorDevice->LastReceivedFlitTime() > result)
			result = t.ProcessorDevice->LastReceivedFlitTime();
	return static_cast<std::size_t>(result);
}
std::size_t GlobalStats::GetMaxBufferStuckDelay() const
{
	double max_delay = -1;
	for (const auto& t : Network.Tiles)
	{
		double delay = t.RouterDevice->stats.GetMaxBufferStuckDelay();
		if (delay > max_delay) max_delay = delay;
	}
	return static_cast<std::size_t>(max_delay);
}
std::size_t GlobalStats::GetMaxTimeFlitInNetwork() const
{
	double result = 0.0;
	for (const auto& t : Network.Tiles)
	{
		if (t.ProcessorDevice->MaxTimeFlitInNetwork() > result)
			result = t.ProcessorDevice->MaxTimeFlitInNetwork();

		for (std::size_t r = 0; r < t.RouterDevice->Size(); r++)
		{
			const auto& relay = (*t.RouterDevice)[r];

			for (std::size_t vc = 0; vc < relay.Size(); vc++)
			{
				const auto& buffer = relay[vc];
				if (!buffer.Empty())
				{
					double span = Config.SimulationTime() - Config.StatsWarmUpTime() - relay[vc].GetOldestAccepted();
					if (span > result) result = span;
				}
			}
		}
	}

	return static_cast<std::size_t>(result);
}

std::size_t GlobalStats::GetPacketsReceived() const
{
	std::int32_t n = 0;

	for (const auto& t : Network.Tiles)
		n += t.ProcessorDevice->PacketsReceived();

	return n;
}
std::size_t GlobalStats::GetFlitsLost() const
{
	std::size_t accepted = GetActualFlitsAccepted();
	std::size_t received = GetActualFlitsReceived();
	std::size_t buffered = GetFlitsInBuffers();
	std::size_t transmitting = GetFlitsInTransmission();
	return accepted - received - buffered - transmitting;
}

double GlobalStats::GetAverageDelay() const
{
	std::size_t total_packets = 0;
	double avg_delay = 0.0;

	for (const auto& t : Network.Tiles)
	{
		std::size_t received_packets = t.ProcessorDevice->PacketsReceived();
		if (received_packets)
		{
			avg_delay += received_packets * t.ProcessorDevice->AverageDelay();
			total_packets += received_packets;
		}
	}

	avg_delay /= (double)total_packets;

	return avg_delay;
}
double GlobalStats::GetMaxDelay() const
{
	double maxd = -1.0;
	for (std::size_t i = 0; i < Network.Tiles.size(); i++)
	{
		double d = -1;
		std::size_t received_packets = Network.Tiles[i].ProcessorDevice->PacketsReceived();
		if (received_packets) d = Network.Tiles[i].ProcessorDevice->MaxDelay();;
		if (d > maxd) maxd = d;
	}
	return maxd;
}

double GlobalStats::GetAverageBufferLoad(std::size_t relay, std::size_t vc) const
{
	double sum = 0;
	for (const auto& t : Network.Tiles)
		sum += t.RouterDevice->stats.GetAverageBufferLoad(relay, vc);
	return sum / Network.Tiles.size();
}
double GlobalStats::GetAverageBufferLoad() const
{
	double sum = 0;
	for (const auto& t : Network.Tiles)
		sum += t.RouterDevice->stats.GetAverageBufferLoad();
	return sum / Network.Tiles.size();
}

double GlobalStats::GetDynamicPower() const
{
	double power = 0.0;
	for (const auto& t : Network.Tiles)
	{
		power += t.RouterDevice->power.getDynamicPower();
	}
	return power;
}
double GlobalStats::GetStaticPower() const
{
	double power = 0.0;

	for (const auto& t : Network.Tiles)
	{
		power += t.RouterDevice->power.getStaticPower();
	}
	return power;
}

void GlobalStats::ShowBuffers(std::ostream& out) const
{
	out << "% Buffer statuses:\n";
	for (std::size_t i = 0; i < Config.TopologyGraph().size(); i++)
	{
		const auto& node = Config.TopologyGraph()[i];
		const auto& stats = Network.Tiles[i].RouterDevice->stats;
		for (std::size_t r = 0; r < Network.Tiles[i].RouterDevice->Size(); r++)
		{
			const auto& relay = (*Network.Tiles[i].RouterDevice)[r];
			for (std::size_t vc = 0; vc < relay.Size(); vc++)
			{
				out << '[';
				out << i << "<-(" << r << ':' << vc << ")--";
				if (r < node.size()) out << node[r] << ']';
				else out << "L]";
				out << ": " << static_cast<std::int32_t>(stats.GetMaxBufferStuckDelay(r, vc)) << " ";
				out << relay[vc] << '\n';
			}
		}
	}
}

std::ostream& operator<<(std::ostream& out, const GlobalStats& gs)
{
	out << "% Total produced flits: " << gs.GetFlitsProduced() << '\n';
	out << "% Total accepted flits: " << gs.GetFlitsAccepted() << '\n';
	out << "% Total received flits: " << gs.GetFlitsReceived() << '\n';
	out << "% Network production (flits/cycle): " << gs.GetProduction() << '\n';
	out << "% Network acceptance (flits/cycle): " << gs.GetAcceptance() << '\n';
	out << "% Network throughput (flits/cycle): " << gs.GetThroughput() << '\n';
	out << "% IP throughput (flits/cycle/IP): " << gs.GetIPThroughput() << '\n';
	out << "% Last time flit received (cycles): " << gs.GetLastReceivedFlitTime() << '\n';
	out << "% Max buffer stuck delay (cycles): " << gs.GetMaxBufferStuckDelay() << '\n';
	out << "% Max time flit in network (cycles): " << gs.GetMaxTimeFlitInNetwork() << '\n';
	out << "% Total received packets: " << gs.GetPacketsReceived() << '\n';
	out << "% Total flits lost: " << gs.GetFlitsLost() << '\n';
	out << "% Global average delay (cycles): " << gs.GetAverageDelay() << '\n';
	out << "% Max delay (cycles): " << gs.GetMaxDelay() << '\n';
	out << "% Average buffer utilization: " << gs.GetAverageBufferLoad() << '\n';
	if (gs.Config.ChannelsCount() > 1)
	{
		//for (std::int32_t i = 0; i < gs.Config.ChannelsCount(); i++)
		//	out << "% Average channel " << i << " utilization: " << gs.GetAverageBufferLoad(i) << '\n';
	}
	out << "% Total energy (J): " << gs.GetTotalPower() << '\n';
	out << "% Dynamic energy (J): " << gs.GetDynamicPower() << '\n';
	out << "% Static energy (J): " << gs.GetStaticPower() << '\n';

	if (gs.Config.ReportBuffers()) gs.ShowBuffers(out);
	return out;
}

