#pragma once
#include "SelectionStrategy.hpp"



class SelectionBufferLevel : public SelectionStrategy
{
public:
	Connection Apply(const Router& router, const Flit& flit, const std::vector<Connection>& directions) const override
	{
		std::vector<Connection> best_dirs;
		std::int32_t max_free_slots = 0;
		for (std::int32_t i = 0; i < directions.size(); i++)
		{
			Connection dst = directions[i];
			if (!router.CanSend(dst)) continue;

			std::int32_t free_slots = router.DestinationFreeSlots(dst);

			if (free_slots > max_free_slots)
			{
				max_free_slots = free_slots;
				best_dirs.clear();
				best_dirs.push_back(dst);
			}
			else if (free_slots == max_free_slots) best_dirs.push_back(dst);
		}

		if (best_dirs.size()) return best_dirs[rand() % best_dirs.size()];
		else return directions[rand() % directions.size()];
	}

};

