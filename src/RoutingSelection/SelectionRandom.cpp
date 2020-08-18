#include "SelectionRandom.hpp"



std::int32_t SelectionRandom::Apply(Router& router, const std::vector<std::int32_t>& directions, const RouteData& route_data) const
{
    assert(directions.size() != 0);
    return directions[rand() % directions.size()];
}
