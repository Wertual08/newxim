#pragma once
#include "RoutingAlgorithm.hpp"
#include "Configuration/Graph/Graph.hpp"



class RoutingTorusAlgorithm : public RoutingAlgorithm {
protected:
	const std::int32_t torus_w, torus_h;
	const Graph& graph;

	std::int32_t GetXFromID(std::int32_t id) const {
		return id % torus_w;
	}
	std::int32_t GetYFromID(std::int32_t id) const {
		return id / torus_w;
	}
	std::int32_t GetIDFromXY(std::int32_t x, std::int32_t y) const {
		if (x >= torus_w) {
			x = 0;
		} else if (x < 0) {
			x = torus_w - 1;
		}
		if (y >= torus_h) {
			y = 0;
		} else if (y < 0) {
			y = torus_h - 1;
		}
		return x + y * torus_w;
	}
	std::vector<std::int32_t> GetLinksTo(std::int32_t id, std::int32_t x, std::int32_t y) const {
		return graph[id].links_to(GetIDFromXY(x, y));
	}

	std::int32_t DistanceN(std::int32_t s, std::int32_t d) {
		if (d >= s) {
			return d - s;
		} else {
			return s + torus_h - d;
		}
	}
	std::int32_t DistanceW(std::int32_t s, std::int32_t d) {
		if (d <= s) {
			return s - d;
		} else {
			return d + torus_w - s;
		}
	}
	std::int32_t DistanceS(std::int32_t s, std::int32_t d) {
		if (d <= s) {
			return s - d;
		} else {
			return d + torus_h - s;
		}
	}
	std::int32_t DistanceE(std::int32_t s, std::int32_t d) {
		if (d >= s) {
			return d - s;
		} else {
			return s + torus_w - d;
		}
	}

public:
	RoutingTorusAlgorithm(std::int32_t w, std::int32_t h, const Graph& g) :
		torus_w(w), torus_h(h), graph(g) {
	}
};
