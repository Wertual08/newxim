#pragma once
#include "RoutingAlgorithm.hpp"
#include "Configuration/Graph/Graph.hpp"



class RoutingMeshXY : public RoutingAlgorithm
{
private:
	const std::int32_t TorusW, TorusH;
	const Graph& TorusGraph;

	std::int32_t GetXFromID(std::int32_t id) const
	{
		return id % TorusW;
	}
	std::int32_t GetYFromID(std::int32_t id) const
	{
		return id / TorusW;
	}
	std::int32_t GetIDFromXY(std::int32_t x, std::int32_t y) const
	{
		if (x >= TorusW) x = 0;
		if (x < 0) x = TorusW - 1;
		if (y >= TorusH) y = 0;
		if (y < 0) y = TorusH - 1;
		return x + y * TorusW;
	}
	std::vector<std::int32_t> GetLinksTo(std::int32_t id, std::int32_t x, std::int32_t y) const
	{
		return TorusGraph[id].links_to(GetIDFromXY(x, y));
	}

public:
	RoutingMeshXY(std::int32_t w, std::int32_t h, const Graph& graph) :
		TorusW(w), TorusH(h), TorusGraph(graph)
	{
	}

	void Route(const Router& router, const Flit& flit, std::vector<Connection>& result) const override
	{
		std::int32_t id = router.LocalID;
		std::int32_t x = GetXFromID(router.LocalID);
		std::int32_t y = GetYFromID(router.LocalID);
		std::int32_t dx = GetXFromID(flit.dst_id) - x;
		std::int32_t dy = GetYFromID(flit.dst_id) - y;

		if (dx > 0) for (std::int32_t l : GetLinksTo(id, x + 1, y)) result.push_back({ l, flit.vc_id });
		else if (dx < 0) for (std::int32_t l : GetLinksTo(id, x - 1, y)) result.push_back({ l, flit.vc_id });
		else if (dy > 0) for (std::int32_t l : GetLinksTo(id, x, y + 1)) result.push_back({ l, flit.vc_id });
		else if (dy < 0) for (std::int32_t l : GetLinksTo(id, x, y - 1)) result.push_back({ l, flit.vc_id });
		else result.push_back({ static_cast<std::int32_t>(TorusGraph[id].size()), flit.vc_id });
	}
};
