#pragma once
#include "Graph.hpp"



class TreeGraph : public Graph
{
public:
	TreeGraph(std::int32_t count, std::int32_t child, std::int32_t channels_count);
};

