#pragma once
#include "RoutingAlgorithm.hpp"
#include "Configuration/Graph.hpp"
#include "Configuration/RoutingTable.hpp"



class RoutingVirtualSubnetwork : public RoutingAlgorithm
{
private:
	const RoutingTable& Table;
	const RoutingTable& SubnetworkTable;


public:
	RoutingVirtualSubnetwork(const RoutingTable& table, const RoutingTable& subnetwork) :
		Table(table), SubnetworkTable(subnetwork)
	{
	}

	std::vector<Connection> Route(const Router& router, const Flit& flit) const override
	{
		std::vector<Connection> result;

		const auto &ports = Table[router.LocalID][flit.dst_id];
		for (auto port : ports)
		{
			Connection con = { port, 0 };
			if (!router.CanSend(con)) continue;
			if (router.DestinationFreeSlots(con) >= 1)
			{
				result.push_back(con);
			}
		}

		if (result.empty())
		{
			const auto &ports = SubnetworkTable[router.LocalID][flit.dst_id];
			for (auto port : ports)
			{
				Connection con = { port, 1 };
				if (!router.CanSend(con)) continue;
				if (router.DestinationFreeSlots(con) >= 1)
				{
					result.push_back(con);
				}
			}
		}

		return result;
	}
};
