#include "SelectionRandom.h"



int32_t SelectionRandom::Apply(Router& router, const std::vector<int32_t>& directions, const RouteData& route_data)
{
    assert(directions.size() != 0);
    return directions[rand() % directions.size()];
}
void SelectionRandom::PerCycleProcess(Router& router) { }
