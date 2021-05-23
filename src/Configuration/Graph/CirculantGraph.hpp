#pragma once
#include "Graph.hpp"



class CirculantGraph : public Graph
{
public:
	CirculantGraph(std::int32_t count, const std::vector<std::int32_t> &generators, std::int32_t channels_count = 1);
};

