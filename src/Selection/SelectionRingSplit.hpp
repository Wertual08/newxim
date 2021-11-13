#pragma once
#include "SelectionStrategy.hpp"



class SelectionRingSplit : public SelectionStrategy 
{
public:
	Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& connections) const override
    {
        if (connections.size() == 0) {
            return Connection();
        }

        Connection max;

        for (std::size_t i = 0; i < connections.size(); i++) {
            if (connections[i].port > max.port) {
                Connection con = connections[i];

                if (router.CanSend(con) && router.DestinationFreeSlots(con) >= 1) {
                    max = con;
                } else {
                    max = Connection();
                }
            }
        }

        return max;
    }
};
