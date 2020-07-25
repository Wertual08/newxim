#include "RoutingTableBased.hpp"



RoutingTableBased::RoutingTableBased(const RoutingTable& table) : Table(table)
{
}

std::vector<int32_t> RoutingTableBased::Route(Router& router, const RouteData& route_data)
{
    return Table[route_data.current_id][route_data.dst_id];
}
