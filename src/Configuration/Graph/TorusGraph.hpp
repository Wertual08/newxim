#pragma once
#include "Graph.hpp"



class TorusGraph : public Graph
{
public:
	TorusGraph(std::int32_t w, std::int32_t h, std::int32_t channels_count);
};

