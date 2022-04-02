#pragma once
#include "RoutingMeshAlgorithm.hpp"



class RoutingMeshOddEven : public RoutingMeshAlgorithm {
public:
	using RoutingMeshAlgorithm::RoutingMeshAlgorithm;

	void Route(const Router& router, const Flit& flit, std::vector<Connection>& result) const override {
		std::int32_t id = router.LocalId;
		std::int32_t x = GetXFromID(id);
		std::int32_t y = GetYFromID(id);
		std::int32_t dx = GetXFromID(flit.dst_id);
		std::int32_t dy = GetYFromID(flit.dst_id);
		std::int32_t ox = dx - x;
		std::int32_t oy = dy - y;
		
		std::int32_t from_id = graph[id][flit.port_in];
		std::int32_t fx = GetXFromID(from_id);
		std::int32_t fy = GetYFromID(from_id);

		if (ox == 0) {
			if (oy > 0) {
				for (std::int32_t l : GetLinksTo(id, x, y + 1)) {
					result.push_back({ l, flit.vc_id });
				}
			} else {
				for (std::int32_t l : GetLinksTo(id, x, y - 1)) {
					result.push_back({ l, flit.vc_id });
				}
			}
		} else {
			if (ox > 0) {
				if (oy == 0) {
					for (std::int32_t l : GetLinksTo(id, x + 1, y)) {
						result.push_back({ l, flit.vc_id });
					}
				} else {
					if (x % 2 != 0 || x == fx) {
						if (oy > 0) {
							for (std::int32_t l : GetLinksTo(id, x, y + 1)) {
								result.push_back({ l, flit.vc_id });
							}
						} else {
							for (std::int32_t l : GetLinksTo(id, x, y - 1)) {
								result.push_back({ l, flit.vc_id });
							}
						}
					}
					if (dx % 2 != 0 || ox != 1) {
						for (std::int32_t l : GetLinksTo(id, x + 1, y)) {
							result.push_back({ l, flit.vc_id });
						}
					}
				}
			} else {
				if (x % 2 == 0) {
					if (oy > 0) {
						for (std::int32_t l : GetLinksTo(id, x, y + 1)) {
							result.push_back({ l, flit.vc_id });
						}
					} else {
						for (std::int32_t l : GetLinksTo(id, x, y - 1)) {
							result.push_back({ l, flit.vc_id });
						}
					}
				}
			}
		}
	}
};
