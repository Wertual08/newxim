#pragma once
#include "SelectionStrategy.hpp"
#include <algorithm>



class SelectionCirculantVirtualRingSplit : public SelectionStrategy 
{
private:
    const Graph& graph;
    std::int32_t Distance(std::int32_t s, std::int32_t d) const {
        return std::min(std::abs(d - s), static_cast<std::int32_t>(graph.size()) - std::abs(d - s));
    }

public:
    SelectionCirculantVirtualRingSplit(const Graph& g) : graph(g) {
    }

	Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& connections) const override {
        if (connections.size() == 0) {
            return Connection();
        }

        Connection max;

        for (std::size_t i = 0; i < connections.size(); i++) {
            if (connections[i].port > max.port) {
                Connection con = connections[i];
                if (flit.port_out != con.port) {
                    con.vc = router.LocalId < graph.size() / 2 ? 0 : 1;
                } else {
                    con.vc = flit.vc_id;
                }

                if (router.CanSend(con) && router.DestinationFreeSlots(con) >= 1) {
                    max = con;
                } else {
                    max = Connection();
                }
            }
        }

        return max;
    }
};
