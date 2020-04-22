#include "SelectionRandom.h"



int SelectionRandom::Apply(Router& router, const std::vector<int32_t>& directions, const RouteData& route_data)
{
    assert(directions.size() != 0);
    int output = directions[rand() % directions.size()];
    return output;
}
void SelectionRandom::PerCycleProcess(Router& router){ }
