#pragma once
#include "RoutingAlgorithm.hpp"
#include "Configuration/RoutingTable.hpp"



class RoutingTableBased : public RoutingAlgorithm
{
private:
	const RoutingTable& Table;

public:
	RoutingTableBased(const RoutingTable& table) : 
        Table(table)
    {
    }

	void Route(const Router& router, const Flit& flit, std::vector<Connection>& result) const override
    {
        for (std::size_t i = 0; i < Table[router.LocalID][flit.dst_id].size(); i++) {
            result[i] = { Table[router.LocalID][flit.dst_id][i], 0 };
        }
    }

};
