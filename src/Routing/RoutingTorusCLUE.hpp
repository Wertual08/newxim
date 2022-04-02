#pragma once
#include "RoutingTorusAlgorithm.hpp"



class RoutingTorusCLUE : public RoutingTorusAlgorithm {
public:
	using RoutingTorusAlgorithm::RoutingTorusAlgorithm;

	void Route(const Router& router, const Flit& flit, std::vector<Connection>& result) const override {
		std::int32_t id = router.LocalId;
		std::int32_t x = GetXFromID(id);
		std::int32_t y = GetYFromID(id);
		std::int32_t dx = GetXFromID(flit.dst_id) - x;
		std::int32_t dy = GetYFromID(flit.dst_id) - y;

		// Rule 1
		if (dx > torus_w / 2 || dx < 0 && dx >= -torus_w / 2) {
			for (std::int32_t l : GetLinksTo(id, x - 1, y)) {
				result.push_back({ l, 0 });
			}
		}
		if (dx < -torus_w / 2 || dx > 0 && dx <= torus_w / 2) {
			for (std::int32_t l : GetLinksTo(id, x + 1, y)) {
				result.push_back({ l, 0 });
			}
		}
		if (dy > torus_h / 2 || dy < 0 && dy >= -torus_h / 2) {
			for (std::int32_t l : GetLinksTo(id, x, y - 1)) {
				result.push_back({ l, 0 });
			}
		}
		if (dy < -torus_h / 2 || dy > 0 && dy <= torus_h / 2) {
			for (std::int32_t l : GetLinksTo(id, x, y + 1)) {
				result.push_back({ l, 0 });
			}
		}

		// Rule 2-3
		if (std::abs(dx) > torus_w / 2) {
			if (x == 0) {
				for (std::int32_t l : GetLinksTo(id, x - 1, y)) {
					result.push_back({ l, 1 });
				}
			}
			if (x == torus_w - 1) {
				for (std::int32_t l : GetLinksTo(id, x + 1, y)) {
					result.push_back({ l, 1 });
				}
			}
		} else if (std::abs(dy) > torus_h / 2) {
			if (y == 0) {
				for (std::int32_t l : GetLinksTo(id, x, y - 1)) {
					result.push_back({ l, 1 });
				}
			}
			if (y == torus_h - 1) {
				for (std::int32_t l : GetLinksTo(id, x, y + 1)) {
					result.push_back({ l, 1 });
				}
			}
		} else {
			if (dx > 0) {
				for (std::int32_t l : GetLinksTo(id, x + 1, y)) {
					result.push_back({ l, 1 });
				}
			} else if (dx < 0) {
				for (std::int32_t l : GetLinksTo(id, x - 1, y)) {
					result.push_back({ l, 1 });
				}
			} else if (dy > 0) {
				for (std::int32_t l : GetLinksTo(id, x, y + 1)) {
					result.push_back({ l, 1 });
				}
			} else if (dy < 0) {
				for (std::int32_t l : GetLinksTo(id, x, y - 1)) {
					result.push_back({ l, 1 });
				}
			}
		}
	}
};
