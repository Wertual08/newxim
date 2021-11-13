#pragma once
#include <algorithm>
#include "RoutingAlgorithm.hpp"
#include "Configuration/Graph/Graph.hpp"
#include "Configuration/RoutingTable.hpp"



class RoutingRingSplit : public RoutingAlgorithm
{
private:
	const Graph &Circulant;
	const RoutingTable &Table;

	std::int32_t Distance(std::int32_t s, std::int32_t d) const
	{
		return std::min(std::abs(d - s), static_cast<std::int32_t>(Circulant.size()) - std::abs(d - s));
	}

public:
	RoutingRingSplit(const Graph &circulant, const RoutingTable &table) :
		Circulant(circulant), Table(table)
	{
	}

	void Route(const Router &router, const Flit &flit, std::vector<Connection>& result) const override
	{
		const auto &ports = Table[router.LocalID][flit.dst_id];

		std::int32_t max_dist = -1;

		for (auto port : ports)
		{
			std::int32_t dist = Distance(router.LocalID, Circulant[router.LocalID][port]);
			if (dist < max_dist) continue;
			if (dist > max_dist)
			{
				result.clear();
				max_dist = dist;
			}

			Connection con = { port, flit.vc_id };
			if (!router.CanSend(con)) continue;

			if (router.DestinationFreeSlots(con) >= 1)
			{
				result.push_back(con);
			}
		}
	}
};
