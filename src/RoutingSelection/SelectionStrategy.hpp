#pragma once
#include <vector>
#include "../DataStructs.hpp"
#include "../Utils.hpp"
#include "Hardware/Connection.hpp"



class Router;
class SelectionStrategy
{
public:
	virtual Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions) const = 0;
};
