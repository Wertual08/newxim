#pragma once
#include <vector>
#include "Data/Flit.hpp"
#include "Hardware/Connection.hpp"



class Router;
class SelectionStrategy
{
public:
	virtual Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions) const = 0;
};
