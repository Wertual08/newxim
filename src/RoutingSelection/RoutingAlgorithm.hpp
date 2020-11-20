#pragma once
#include <vector>
#include "../DataStructs.hpp"
#include "../Utils.hpp"
#include "Hardware/Connection.hpp"



class Router;
class RoutingAlgorithm
{
public:
	virtual std::vector<Connection> Route(const Router& router, const Flit& flit) const = 0;
};
