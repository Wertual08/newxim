#pragma once
#include "RoutingMeshAlgorithm.hpp"



class RoutingMeshO1TURN : public RoutingMeshAlgorithm {
public:
	using RoutingMeshAlgorithm::RoutingMeshAlgorithm;

	void Route(const Router& router, const Flit& flit, std::vector<Connection>& result) const override {
		std::int32_t id = router.LocalId;
		std::int32_t x = GetXFromID(router.LocalId);
		std::int32_t y = GetYFromID(router.LocalId);
		std::int32_t dx = GetXFromID(flit.dst_id) - x;
		std::int32_t dy = GetYFromID(flit.dst_id) - y;

		std::int32_t vc = flit.vc_id;
		if (flit.src_id == id) {
			vc = rand() % 2;
		}

		if (vc == 0) {
			if (dx > 0) {
				for (std::int32_t l : GetLinksTo(id, x + 1, y)) {
					result.push_back({ l, vc });
				}
			} else if (dx < 0) {
				for (std::int32_t l : GetLinksTo(id, x - 1, y)) {
					result.push_back({ l, vc });
				}
			} else if (dy > 0) {
				for (std::int32_t l : GetLinksTo(id, x, y + 1)) {
					result.push_back({ l, vc });
				}
			} else if (dy < 0) {
				for (std::int32_t l : GetLinksTo(id, x, y - 1)) {
					result.push_back({ l, vc });
				}
			}
		} else {
			if (dy > 0) {
				for (std::int32_t l : GetLinksTo(id, x, y + 1)) {
					result.push_back({ l, vc });
				}
			} else if (dy < 0) {
				for (std::int32_t l : GetLinksTo(id, x, y - 1)) {
					result.push_back({ l, vc });
				}
			} else if (dx > 0) {
				for (std::int32_t l : GetLinksTo(id, x + 1, y)) {
					result.push_back({ l, vc });
				}
			} else if (dx < 0) {
				for (std::int32_t l : GetLinksTo(id, x - 1, y)) {
					result.push_back({ l, vc });
				}
			}
		}
	}
};
