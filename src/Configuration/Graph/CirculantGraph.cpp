#include "CirculantGraph.hpp"



CirculantGraph::CirculantGraph(std::int32_t count, const std::vector<std::int32_t>& generators, std::int32_t channels_count)
{
	resize(count);
	for (std::int32_t i = 0; i < generators.size(); i++)
	{
		std::int32_t l = generators[i];
		for (std::int32_t j = 0; j < size(); j++)
		{
			std::int32_t next = (j + l) % size();
			std::int32_t prev = j - l;
			if (prev < 0) prev += size();
			at(j).push_back(next, channels_count);
			at(j).push_back(prev, channels_count);
		}
	}
}
