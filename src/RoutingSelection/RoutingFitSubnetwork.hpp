#pragma once
#include "RoutingAlgorithm.hpp"
#include "Configuration/Graph.hpp"
#include "../Configuration/RoutingTable.hpp"



class RoutingFitSubnetwork : public RoutingAlgorithm
{
private:
	const RoutingTable& Table;
	const RoutingTable& SubnetworkTable;


public:
	RoutingFitSubnetwork(const RoutingTable& table, const RoutingTable& subnetwork);

	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};