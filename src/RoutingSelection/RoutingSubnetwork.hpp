#pragma once
#include "RoutingAlgorithm.hpp"
#include "Configuration/Graph.hpp"
#include "../Configuration/RoutingTable.hpp"



class RoutingSubnetwork : public RoutingAlgorithm
{
private:
	const RoutingTable& Table;
	const RoutingTable& SubnetworkTable;


public:
	RoutingSubnetwork(const RoutingTable& table, const RoutingTable& subnetwork);

	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};
