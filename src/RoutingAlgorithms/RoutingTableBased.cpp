#include "RoutingTableBased.h"



std::vector<int32_t> RoutingTableBased::Route(Router& router, const RouteData& routeData)
{
    return router.routing_table[routeData.dst_id];
}
