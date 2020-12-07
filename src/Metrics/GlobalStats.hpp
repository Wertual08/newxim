/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global statistics
 */

#pragma once
#include <systemc.h>
#include <vector>
#include <map>
#include "Configuration/Configuration.hpp"
#include "Hardware/NoC.hpp"



class GlobalStats : public sc_module
{
	SC_HAS_PROCESS(GlobalStats);

private:
	const Configuration& Config;
	const NoC& Network;

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

	double GetTotalPower() const { return GetDynamicPower() + GetStaticPower(); }
	double GetDynamicPower() const;
	double GetStaticPower() const;

	void ShowBuffers(std::ostream& out) const;

	void Update();

	void FinishStats() const;

	GlobalStats(sc_module_name name, const NoC& network, const Configuration& config);

public:
	sc_in_clk clock; 
	sc_in<bool> reset;
	
	GlobalStats(const NoC& network, const Configuration& config) :
		GlobalStats("GS", network, config) { } 

	// Shows global statistics
	friend std::ostream& operator<<(std::ostream& out, const GlobalStats& gs);
};