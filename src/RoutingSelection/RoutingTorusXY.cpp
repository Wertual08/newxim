#include "RoutingTorusXY.hpp"
#include "../Hardware/Routers/Router.hpp"



std::int32_t RoutingTorusXY::GetXFromID(std::int32_t id) const
{
    return id % TorusW;
}
std::int32_t RoutingTorusXY::GetYFromID(std::int32_t id) const
{
    return id / TorusW;
}
std::int32_t RoutingTorusXY::GetIDFromXY(std::int32_t x, std::int32_t y) const
{
    if (x >= TorusW) x = 0;
    if (x < 0) x = TorusW - 1;
    if (y >= TorusH) y = 0;
    if (y < 0) y = TorusH - 1;
    return x + y * TorusW;
}
std::vector<std::int32_t> RoutingTorusXY::GetLinksTo(std::int32_t id, std::int32_t x, std::int32_t y) const
{
    return TorusGraph[id].links_to(GetIDFromXY(x, y));
}

RoutingTorusXY::RoutingTorusXY(std::int32_t w, std::int32_t h, const Graph& graph) :
    TorusW(w), TorusH(h), TorusGraph(graph)
{
}

std::vector<std::int32_t> RoutingTorusXY::Route(Router& router, Flit& flit) const
{
    std::int32_t prev_id = router.LocalID;
    if (flit.dir_in < TorusGraph[router.LocalID].size())
        prev_id = TorusGraph[router.LocalID][flit.dir_in];
    std::int32_t prev_vc = 0;
    for (std::int32_t i = 0; i < TorusGraph[router.LocalID].links_to(prev_id).size(); i++)
        if (TorusGraph[router.LocalID].links_to(prev_id)[i] == flit.dir_in) prev_vc = i;

    std::int32_t id = router.LocalID;
    std::int32_t x = GetXFromID(router.LocalID);
    std::int32_t y = GetYFromID(router.LocalID);
    std::int32_t dx = GetXFromID(flit.dst_id) - x;
    std::int32_t dy = GetYFromID(flit.dst_id) - y;

    std::int32_t ax = abs(dx) * 2;
    std::int32_t ay = abs(dy) * 2;

    if (ax > TorusW)
    {
        dx = -dx; 
    }
    if (ay > TorusH)
    {
        dy = -dy;
    }

    std::vector<std::int32_t> result; 
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
