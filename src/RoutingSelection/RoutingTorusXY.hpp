#pragma once
#include "RoutingAlgorithm.hpp"
#include "Configuration/Graph.hpp"



class RoutingTorusXY : public RoutingAlgorithm
{
private:
	const std::int32_t TorusW, TorusH;
	const Graph& TorusGraph;

	std::int32_t GetXFromID(std::int32_t id) const;
	std::int32_t GetYFromID(std::int32_t id) const;
	std::int32_t GetIDFromXY(std::int32_t x, std::int32_t y) const;
	std::vector<std::int32_t> GetLinksTo(std::int32_t id, std::int32_t x, std::int32_t y) const;

public:
	RoutingTorusXY(std::int32_t w, std::int32_t h, const Graph& graph);

	std::vector<std::int32_t> Route(Router& router, Flit& flit) const override;
};
