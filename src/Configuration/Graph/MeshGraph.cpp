#include "MeshGraph.hpp"



MeshGraph::MeshGraph(std::int32_t w, std::int32_t h, std::int32_t channels_count)
{
	resize(w * h);
	for (std::int32_t x = 0; x < w; x++) {
		for (std::int32_t y = 0; y < h; y++) {
			if (y + 1 < h) {
				at(y * w + x).push_back((y + 1) * w + x, channels_count);
			}
			if (x - 1 >= 0) {
				at(y * w + x).push_back(y * w + x - 1, channels_count);
			}
			if (y - 1 >= 0) {
				at(y * w + x).push_back((y - 1) * w + x, channels_count);
			}
			if (x + 1 < w) {
				at(y * w + x).push_back(y * w + x + 1, channels_count);
			}
		}
	}
}
