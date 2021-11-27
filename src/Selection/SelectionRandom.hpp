#pragma once
#include "SelectionStrategy.hpp"



class SelectionRandom : public SelectionStrategy 
{
public:
	Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions) const override
    {
        if (directions.size() == 0) {
            return Connection();
        } else {
            return directions[rand() % directions.size()];
        }
    }

};
