#pragma once
#include "SelectionStrategy.hpp"



class SelectionRandomKeepSpace : public SelectionStrategy
{
public:
	Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions) const override
	{
		std::vector<Connection> best_dirs;
		for (std::int32_t i = 0; i < directions.size(); i++)
		{
			Connection dst = directions[i];
			std::int32_t free_slots = router[dst.port].GetFreeSlots(flit.vc_id);

			if (flit.dir_in != router.LocalRelayID) best_dirs.push_back(dst);
			else if (free_slots >= router[flit.dir_in][flit.vc_id].GetCapacity() - 0) best_dirs.push_back(dst);
		}

		if (best_dirs.size() > 0) return best_dirs[rand() % best_dirs.size()];
		else return Connection();
	}

};

