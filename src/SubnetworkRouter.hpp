#pragma once
#include "Router.hpp"



class SubnetworkRouter : public Router
{
protected:
	std::vector<std::vector<std::int32_t>> SubnetworkTable;

public:
	using Router::Router;
	void SetupSubnetworkTable(const std::vector<std::vector<std::int32_t>>& table)
	{
		SubnetworkTable = table;
	}
};