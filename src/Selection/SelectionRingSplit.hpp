#pragma once
#include "SelectionStrategy.hpp"



class SelectionRingSplit : public SelectionStrategy 
{
public:
	Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& connections) const override
    {
        if (connections.size() == 0) return Connection();
        else return connections[rand() % connections.size()];
    }
};
