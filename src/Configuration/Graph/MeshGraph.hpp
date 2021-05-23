#pragma once
#include "Graph.hpp"



class MeshGraph : public Graph
{
public:
	MeshGraph(std::int32_t w, std::int32_t h, std::int32_t channels_count);
};

