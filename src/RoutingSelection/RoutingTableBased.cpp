#include "RoutingTableBased.hpp"



RoutingTableBased::RoutingTableBased(const RoutingTable& table) : Table(table)
{
}

std::vector<std::int32_t> RoutingTableBased::Route(Router& router, Flit& flit) const
{
    return Table[router.LocalID][flit.dst_id];
}
