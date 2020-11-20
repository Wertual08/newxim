#include "RoutingTableBased.hpp"



RoutingTableBased::RoutingTableBased(const RoutingTable& table) : Table(table)
{
}

std::vector<Connection> RoutingTableBased::Route(const Router& router, const Flit& flit) const
{
    std::vector<Connection> result(Table[router.LocalID][flit.dst_id].size());
    for (std::size_t i = 0; i < Table[router.LocalID][flit.dst_id].size(); i++)
        result[i] = { Table[router.LocalID][flit.dst_id][i], 0 };
    return result;
}
