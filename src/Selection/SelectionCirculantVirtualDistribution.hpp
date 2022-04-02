#pragma once
#include "SelectionStrategy.hpp"
#include <algorithm>



class SelectionCirculantVirtualDistribution : public SelectionStrategy
{
private:
    const Graph& graph;
    std::int32_t Distance(std::int32_t s, std::int32_t d) const {
        return std::min(std::abs(d - s), static_cast<std::int32_t>(graph.size()) - std::abs(d - s));
    }

public:
    SelectionCirculantVirtualDistribution(const Graph& g) : graph(g) {
    }

    Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& connections) const override {
        if (connections.size() == 0) {
            return Connection();
        }

        Connection con = connections[rand() % connections.size()];

        std::int32_t id = router.LocalId;

        if (flit.src_id != id && id % 2 != 0) {
            con.vc = flit.vc_id == 0 ? 1 : 0;
        } else {
            con.vc = flit.vc_id;
        }

        //con.vc = flit.src_id % 2;

        return con;
    }
};