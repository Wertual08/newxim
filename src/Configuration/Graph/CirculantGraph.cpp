#include "CirculantGraph.hpp"



CirculantGraph::CirculantGraph(std::int32_t count, const std::vector<std::int32_t>& generators, std::int32_t channels_count)
{
	resize(count);
	for (std::int32_t i = 0; i < generators.size(); i++)
	{
		std::int32_t l = generators[i];
		for (std::int32_t j = 0; j < size(); j++)
		{
			at(j).push_back((j + l) % size(), channels_count);
			at((j + l) % size()).push_back(j, channels_count);
		}
	}
}
