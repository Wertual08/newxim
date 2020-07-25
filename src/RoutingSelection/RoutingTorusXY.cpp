#include "RoutingTorusXY.hpp"



int32_t RoutingTorusXY::GetXFromID(int32_t id)
{
    return id % TorusW;
}
int32_t RoutingTorusXY::GetYFromID(int32_t id)
{
    return id / TorusW;
}
int32_t RoutingTorusXY::GetIDFromXY(int32_t x, int32_t y)
{
    if (x >= TorusW) x = 0;
    if (x < 0) x = TorusW - 1;
    if (y >= TorusH) y = 0;
    if (y < 0) y = TorusH - 1;
    return x + y * TorusW;
}
std::vector<int32_t> RoutingTorusXY::GetLinksTo(int32_t id, int32_t x, int32_t y)
{
    return TorusGraph[id].links_to(GetIDFromXY(x, y));
}

RoutingTorusXY::RoutingTorusXY(int32_t w, int32_t h, const Graph& graph) :
    TorusW(w), TorusH(h), TorusGraph(graph)
{
}

std::vector<int32_t> RoutingTorusXY::Route(Router& router, const RouteData& route_data)
{
    int32_t prev_id = route_data.current_id;
    if (route_data.dir_in < TorusGraph[route_data.current_id].size())
        prev_id = TorusGraph[route_data.current_id][route_data.dir_in];
    int32_t prev_vc = 0;
    for (int32_t i = 0; i < TorusGraph[route_data.current_id].links_to(prev_id).size(); i++)
        if (TorusGraph[route_data.current_id].links_to(prev_id)[i] == route_data.dir_in) prev_vc = i;

    int32_t id = route_data.current_id;
    int32_t x = GetXFromID(route_data.current_id);
    int32_t y = GetYFromID(route_data.current_id);
    int32_t dx = GetXFromID(route_data.dst_id) - x;
    int32_t dy = GetYFromID(route_data.dst_id) - y;

    int32_t ax = abs(dx) * 2;
    int32_t ay = abs(dy) * 2;

    if (ax > TorusW)
    {
        dx = -dx; 
    }
    if (ay > TorusH)
    {
        dy = -dy;
    }

    std::vector<int32_t> result; 
    if (dx > 0)
    {
        if (x + 1 >= TorusW) prev_vc += 1;
        result.push_back(GetLinksTo(id, x + 1, y)[prev_vc]);
    }
    else if (dx < 0)
    {
        if (x - 1 < 0) prev_vc += 1;
        result.push_back(GetLinksTo(id, x - 1, y)[prev_vc]);
    }
    else if (dy > 0)
    {
        if (y + 1 >= TorusH) prev_vc += 1;
        result.push_back(GetLinksTo(id, x, y + 1)[prev_vc]);
    }
    else if (dy < 0)
    {
        if (y - 1 < 0) prev_vc += 1;
        result.push_back(GetLinksTo(id, x, y - 1)[prev_vc]);
    }
    else result.push_back(TorusGraph[id].size());

    return result;
}
