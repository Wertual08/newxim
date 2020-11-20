#include "SelectionRandom.hpp"



Connection SelectionRandom::Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions) const
{
    if (directions.size() == 0) return Connection();
    else return directions[rand() % directions.size()];
}
