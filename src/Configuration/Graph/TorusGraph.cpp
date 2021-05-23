#include "MeshGraph.hpp"
#include "TorusGraph.hpp"



TorusGraph::TorusGraph(std::int32_t w, std::int32_t h, std::int32_t channels_count)
{
	resize(w * h);
	for (std::int32_t x = 0; x < w; x++)
	{
		for (std::int32_t y = 0; y < h; y++)
		{
			auto& node = at(y * w + x);
			if (y + 1 < h) node.push_back((y + 1) * w + x, channels_count);
			else node.push_back(x, channels_count);
			if (x - 1 >= 0) node.push_back(y * w + x - 1, channels_count);
			else node.push_back(y * w + w - 1, channels_count);
			if (y - 1 >= 0) node.push_back((y - 1) * w + x, channels_count);
			else node.push_back((h - 1) * w + x, channels_count);
			if (x + 1 < w) node.push_back(y * w + x + 1, channels_count);
			else node.push_back(y * w, channels_count);
		}
	}
}
