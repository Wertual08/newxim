#include "SelectionRandom.hpp"



std::int32_t SelectionRandom::Apply(Router& router, Flit& flit, const std::vector<std::int32_t>& directions) const
{
    if (directions.size() == 0) return -1;
    else return directions[rand() % directions.size()];
}
