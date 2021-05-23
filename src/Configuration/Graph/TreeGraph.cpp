#include "MeshGraph.hpp"
#include "TreeGraph.hpp"



TreeGraph::TreeGraph(std::int32_t count, std::int32_t child, std::int32_t channels_count)
{
	resize(count);
	for (std::int32_t i = 0; i < size(); i++)
	{
		if (i * child + 1 >= size()) break;
		for (std::int32_t j = 1; j <= child; j++)
		{
			if (i * child + j >= size()) break;
			at(i).push_back(i * child + j, channels_count);
			at(i * child + j).push_back(i, channels_count);
		}
	}
}
