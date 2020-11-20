#include "RoutingSubnetwork.hpp"
#include "../Hardware/Routers/Router.hpp"



RoutingSubnetwork::RoutingSubnetwork(const RoutingTable& table, const RoutingTable& subnetwork) :
	Table(table), SubnetworkTable(subnetwork)
{
}

std::vector<Connection> RoutingSubnetwork::Route(const Router& router, const Flit& flit) const
{
	std::vector<Connection> result;

	const auto& ports = Table[router.LocalID][flit.dst_id];
	for (auto port : ports)
	{
		Connection con = { port, 0 };
		if (router.DestinationFreeSlots(con) >= 1)
		{
			result.push_back(con);
		}
	}

	if (result.empty())
	{
		const auto& ports = SubnetworkTable[router.LocalID][flit.dst_id];
		for (auto port : ports)
		{
			Connection con = { port, 1 };
			if (router.DestinationFreeSlots(con) >= 1)
			{
				result.push_back(con);
			}
		}
	}

	return result;
}
