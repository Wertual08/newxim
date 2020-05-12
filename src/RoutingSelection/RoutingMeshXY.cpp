#include "RoutingMeshXY.h"



int32_t RoutingMeshXY::GetXFromID(int32_t id)
{
    return id % TorusW;
}
int32_t RoutingMeshXY::GetYFromID(int32_t id)
{
    return id / TorusW;
}
int32_t RoutingMeshXY::GetIDFromXY(int32_t x, int32_t y)
{
    if (x >= TorusW) x = 0;
    if (x < 0) x = TorusW - 1;
    if (y >= TorusH) y = 0;
    if (y < 0) y = TorusH - 1;
    return x + y * TorusW;
}
std::vector<int32_t> RoutingMeshXY::GetLinksTo(int32_t id, int32_t x, int32_t y)
{
    return TorusGraph[id].links_to(GetIDFromXY(x, y));
}

RoutingMeshXY::RoutingMeshXY(int32_t w, int32_t h, const Graph& graph) :
    TorusW(w), TorusH(h), TorusGraph(graph)
{
}

std::vector<int32_t> RoutingMeshXY::Route(Router& router, const RouteData& route_data)
{
    int32_t id = route_data.current_id;
    int32_t x = GetXFromID(route_data.current_id);
    int32_t y = GetYFromID(route_data.current_id);
    int32_t dx = GetXFromID(route_data.dst_id) - x;
    int32_t dy = GetYFromID(route_data.dst_id) - y;

    std::vector<int32_t> result;
    if (dx > 0) result.push_back(GetLinksTo(id, x + 1, y)[0]);
    else if (dx < 0) result.push_back(GetLinksTo(id, x - 1, y)[0]);
    else if (dy > 0) result.push_back(GetLinksTo(id, x, y + 1)[0]);
    else if (dy < 0) result.push_back(GetLinksTo(id, x, y - 1)[0]);
    else result.push_back(TorusGraph[id].size());

    return result;
}
