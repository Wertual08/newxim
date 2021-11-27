#pragma once
#include "RoutingAlgorithm.hpp"
#include "Configuration/Graph/Graph.hpp"



class RoutingMeshAlgorithm : public RoutingAlgorithm {
protected:
	const std::int32_t mesh_w, mesh_h;
	const Graph& graph;

	std::int32_t GetXFromID(std::int32_t id) const {
		return id % mesh_w;
	}
	std::int32_t GetYFromID(std::int32_t id) const {
		return id / mesh_w;
	}
	std::int32_t GetIDFromXY(std::int32_t x, std::int32_t y) const {
		if (x >= mesh_w) {
			x = 0;
		} else if (x < 0) {
			x = mesh_w - 1;
		}
		if (y >= mesh_h) {
			y = 0;
		} else if (y < 0) {
			y = mesh_h - 1;
		}
		return x + y * mesh_w;
	}
	std::vector<std::int32_t> GetLinksTo(std::int32_t id, std::int32_t x, std::int32_t y) const {
		return graph[id].links_to(GetIDFromXY(x, y));
	}

public:
	RoutingMeshAlgorithm(std::int32_t w, std::int32_t h, const Graph& g) :
		mesh_w(w), mesh_h(h), graph(g) {
	}
};
