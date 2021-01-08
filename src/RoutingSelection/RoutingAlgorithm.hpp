#pragma once
#include <vector>
#include "DataStructs.hpp"
#include "Hardware/Connection.hpp"
#include "Hardware/Router.hpp"



class RoutingAlgorithm
{
public:
	virtual std::vector<Connection> Route(const Router& router, const Flit& flit) const = 0;
};
