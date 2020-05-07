#include "SelectionTorusAdaptive.h"



int32_t SelectionTorusAdaptive::GetXFromID(int32_t id)
{
    return id % TorusW;
}

int32_t SelectionTorusAdaptive::GetYFromID(int32_t id)
{
    return id / TorusW;
}

SelectionTorusAdaptive::SelectionTorusAdaptive(int32_t w, int32_t h) :
    TorusW(w), TorusH(h)
{
}

int32_t SelectionTorusAdaptive::Apply(Router& router, const std::vector<int32_t>& directions, const RouteData& route_data)
{
    int32_t dx = GetXFromID(route_data.dst_id) - GetXFromID(route_data.current_id);
    int32_t dy = GetYFromID(route_data.dst_id) - GetYFromID(route_data.current_id);

    int32_t ax = abs(dx) * 2;
    int32_t ay = abs(dy) * 2;

    if (ax > TorusW && ay > TorusH)
    {

    }
    else if (ax > TorusW && ay == 0)
    {

    }
    else if (ax > TorusW && ay <= TorusH)
    {

    }
    else if (ax == 0 && ay > TorusH)
    {

    }
    else if (ax == 0 && ay <= TorusH)
    {

    }
    else if (ax == 0 && ay == 0)
    {

    }
    else if (ax <= TorusW && ay > TorusH)
    {

    }
    else if (ax <= TorusW && ay == 0)
    {

    }
    else if (ax <= TorusW && ay <= TorusH)
    {

    }
}
void SelectionTorusAdaptive::PerCycleProcess(Router& router) { }
