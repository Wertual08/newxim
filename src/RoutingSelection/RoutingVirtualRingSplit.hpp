#pragma once
#include <algorithm>
#include "RoutingAlgorithm.hpp"
#include "Configuration/Graph.hpp"
#include "Configuration/RoutingTable.hpp"



class RoutingVirtualRingSplit : public RoutingAlgorithm
{
private:
	const Graph &Circulant;
	const RoutingTable &Table;

	std::int32_t Distance(std::int32_t s, std::int32_t d) const
	{
		return std::min(std::abs(d - s), static_cast<std::int32_t>(Circulant.size()) - 1 - std::abs(d - s));
	}

public:
	RoutingVirtualRingSplit(const Graph &circulant, const RoutingTable &table) :
		Circulant(circulant), Table(table)
	{
	}

	std::vector<Connection> Route(const Router &router, const Flit &flit) const override
	{
		std::vector<Connection> result;

		const auto &ports = Table[router.LocalID][flit.dst_id];

		std::int32_t max_dist = -1;

		for (auto port : ports)
		{
			std::int32_t dist = Distance(router.LocalID, Circulant[router.LocalID][port]);
			std::int32_t from_dist = flit.dir_in != router.LocalRelayID ? Distance(router.LocalID, Circulant[router.LocalID][flit.dir_in]) : -1;
			if (dist < max_dist) continue;
			if (dist > max_dist)
			{
				result.clear();
				max_dist = dist;
			}

			std::int32_t vc = flit.vc_id;
			if (from_dist != dist)
				vc = router.LocalID < Circulant.size() / 2 ? 0 : 1;

			Connection con = { port, vc };
			if (!router.CanSend(con)) continue;

			//if (
			//	//router.LocalID == 0 * Circulant.size() / 4 || 
			//	//router.LocalID == 1 * Circulant.size() / 4 ||
			//	//router.LocalID == 2 * Circulant.size() / 4 || 
			//	//router.LocalID == 3 * Circulant.size() / 4
			//	router.LocalID % 2 == 0
			//) con.vc = !con.vc;

			if (router.DestinationFreeSlots(con) >= 1)
			{
				result.push_back(con);
			}
		}
		

		return result;
	}
};
