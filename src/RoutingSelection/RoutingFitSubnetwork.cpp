#include "RoutingFitSubnetwork.hpp"
#include "../Hardware/Routers/Router.hpp"



RoutingFitSubnetwork::RoutingFitSubnetwork(const RoutingTable& table, const RoutingTable& subnetwork) :
	Table(table), SubnetworkTable(subnetwork)
{
}

std::vector<std::int32_t> RoutingFitSubnetwork::Route(Router& router, Flit& flit) const
{
	std::vector<std::int32_t> result;

	const auto& ports = Table[router.LocalID][flit.dst_id];
	for (auto port : ports)
	{
		if (router[port].GetFreeSlots(0) >= flit.sequence_length)
		{
			result.push_back(port);
		}
	}

	if (result.empty())
	{
		const auto& ports = SubnetworkTable[router.LocalID][flit.dst_id];
		for (auto port : ports)
		{
			if (router[port].GetFreeSlots(1) >= flit.sequence_length)
			{
				result.push_back(port);
			}
		}
		flit.vc_id = 1;
	}
	else flit.vc_id = 0;

	return result;
}
