#pragma once
#include "Routing/RoutingAlgorithm.hpp"
#include "Selection/SelectionStrategy.hpp"
#include "Configuration/TrafficManagers/TrafficManager.hpp"



class Configuration;
class Factory
{
private:
	const Configuration& config;
public:
	Factory(const Configuration& cfg) : config(cfg) {
	}

	std::unique_ptr<RoutingAlgorithm> MakeAlgorithm() const;
	std::unique_ptr<SelectionStrategy> MakeStrategy() const;
	std::unique_ptr<TrafficManager> MakeTraffic() const;
};

