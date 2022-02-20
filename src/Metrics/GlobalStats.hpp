
#pragma once
#include <systemc.h>
#include <vector>
#include <map>
#include "Configuration/Configuration.hpp"
#include "Hardware/Network.hpp"



class GlobalStats : public sc_module
{
	SC_HAS_PROCESS(GlobalStats);

private:
	const Configuration& Config;
	const Network& Network;

	std::size_t GetActualFlitsReceived() const;
	std::size_t GetActualFlitsAccepted() const;
	std::size_t GetFlitsInBuffers() const;
	std::size_t GetFlitsInTransmission() const;


	std::size_t GetFlitsProduced() const;
	std::size_t GetFlitsAccepted() const;
	std::size_t GetFlitsReceived() const;
	double GetProduction() const;
	double GetAcceptance() const;
	double GetThroughput() const;
	double GetIPThroughput() const;

	std::size_t GetLastReceivedFlitTime() const;
	std::size_t GetMaxBufferStuckDelay() const;
	std::size_t GetMaxTimeFlitInNetwork() const;

	std::size_t GetPacketsReceived() const;
	std::size_t GetFlitsLost() const;

	double GetAverageDelay() const;
	double GetMaxDelay() const;


	double GetAverageBufferLoad(std::size_t relay, std::size_t vc) const;
	double GetAverageBufferLoad() const;

	void ShowBuffers(std::ostream& out) const;

	void Update();

	void FinishStats() const;

	GlobalStats(sc_module_name name, const ::Network& network, const Configuration& config);

public:
	sc_in_clk clock; 
	sc_in<bool> reset;
	
	GlobalStats(const ::Network& network, const Configuration& config) :
		GlobalStats("GS", network, config) { } 

	// Shows global statistics
	friend std::ostream& operator<<(std::ostream& out, const GlobalStats& gs);
};
