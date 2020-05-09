#include "RoutingTorusAdaptive.h"



int32_t RoutingTorusAdaptive::GetXFromID(int32_t id)
{
    return id % TorusW;
}
int32_t RoutingTorusAdaptive::GetYFromID(int32_t id)
{
    return id / TorusW;
}
int32_t RoutingTorusAdaptive::GetIDFromXY(int32_t x, int32_t y)
{
    if (x >= TorusW) x = 0;
    if (x < 0) x = TorusW - 1;
    if (y >= TorusH) y = 0;
    if (y < 0) y = TorusH - 1;
    return x + y * TorusW;
}
int32_t RoutingTorusAdaptive::GetLinkTo(int32_t id, int32_t x, int32_t y)
{
    return TorusGraph[id].links_to(GetIDFromXY(x, y))[0];
}

RoutingTorusAdaptive::RoutingTorusAdaptive(int32_t w, int32_t h, const Graph& graph) :
    TorusW(w), TorusH(h), TorusGraph(graph)
{
}

std::vector<int32_t> RoutingTorusAdaptive::Route(Router& router, const RouteData& route_data)
{
    int32_t vc = 0;
    int32_t id = route_data.current_id;
    int32_t x = GetXFromID(route_data.current_id);
    int32_t y = GetYFromID(route_data.current_id);
    int32_t dx = GetXFromID(route_data.dst_id) - x;
    int32_t dy = GetYFromID(route_data.dst_id) - y;

    int32_t ax = abs(dx) * 2;
    int32_t ay = abs(dy) * 2;

    //if (ax >= TorusW)
    //{
    //    dx = -dx;
    //    vc = 1;
    //}
    //if (ay >= TorusH)
    //{
    //    dy = -dy;
    //    if (dx == 0) vc = 1;
    //}

    std::vector<int32_t> result; 
    if (dx > 0) result.push_back(GetLinkTo(id, x + 1, y));
    else if (dx < 0) result.push_back(GetLinkTo(id, x - 1, y));
    else if (dy > 0) result.push_back(GetLinkTo(id, x, y + 1));
    else if (dy < 0) result.push_back(GetLinkTo(id, x, y - 1));
    else result.push_back(TorusGraph[id].size());
    //if (ax > TorusW && ay > TorusH)
    //{
    //    result.push_back(std::make_pair(GetLinkTo(id, x - 1, y), 0));
    //    result.push_back(std::make_pair(GetLinkTo(id, x, y - 1), 0));
    //}
    //else if (ax > TorusW && ay == 0)
    //{
    //    result.push_back(std::make_pair(GetLinkTo(id, x - 1, y), 0));
    //}
    //else if (ax > TorusW && ay <= TorusH)
    //{
    //    result.push_back(std::make_pair(GetLinkTo(id, x - 1, y), 0));
    //    result.push_back(std::make_pair(GetLinkTo(id, x, y + 1), 0));
    //}
    //else if (ax == 0 && ay > TorusH)
    //{
    //    result.push_back(std::make_pair(GetLinkTo(id, x, y - 1), 0));
    //}
    //else if (ax == 0 && ay <= TorusH)
    //{
    //    result.push_back(std::make_pair(GetLinkTo(id, x, y + 1), 0));
    //}
    //else if (ax == 0 && ay == 0)
    //{
    //    result.push_back(std::make_pair(TorusGraph[id].size(), 0));
    //}
    //else if (ax <= TorusW && ay > TorusH)
    //{
    //    result.push_back(std::make_pair(GetLinkTo(id, x + 1, y), 0));
    //    result.push_back(std::make_pair(GetLinkTo(id, x, y - 1), 0));
    //}
    //else if (ax <= TorusW && ay == 0)
    //{
    //    result.push_back(std::make_pair(GetLinkTo(id, x + 1, y), 0));
    //}
    //else if (ax <= TorusW && ay <= TorusH)
    //{
    //    result.push_back(std::make_pair(GetLinkTo(id, x + 1, y), 0));
    //    result.push_back(std::make_pair(GetLinkTo(id, x, y + 1), 0));
    //}
    return result;
}
