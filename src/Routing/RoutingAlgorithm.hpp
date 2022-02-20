#pragma once
#include <vector>
#include "Data/Flit.hpp"
#include "Hardware/Connection.hpp"
#include "Hardware/Router.hpp"



class RoutingAlgorithm
{
public:
	virtual void Route(const Router& router, const Flit& flit, std::vector<Connection>& result) const = 0;
};
