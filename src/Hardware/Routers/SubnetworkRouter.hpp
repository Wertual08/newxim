#pragma once
#include "Hardware/ReservationTable.hpp"
#include "Router.hpp"



class SubnetworkRouter : public Router
{
protected:
	std::vector<std::vector<std::int32_t>> SubnetworkTable;

	virtual void TXProcess() override;

public:
	using Router::Router;

	void SetupSubnetworkTable(const std::vector<std::vector<std::int32_t>> &table)
	{
		SubnetworkTable = table;
	}
};