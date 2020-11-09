#include "SelectionKeepSpace.hpp"
#include "Hardware/Routers/Router.hpp"



std::int32_t SelectionKeepSpace::Apply(Router& router, Flit& flit, const std::vector<std::int32_t>& directions) const
{
	std::vector<std::int32_t> best_dirs;
	std::int32_t max_free_slots = 0;
	for (std::int32_t i = 0; i < directions.size(); i++)
	{
		std::int32_t dir = directions[i];
		std::int32_t free_slots = router[dir][flit.vc_id].GetFreeSlots();

		if (flit.dir_in != router.LocalRelayID)
		{
			if (free_slots > max_free_slots)
			{
				max_free_slots = free_slots;
				best_dirs.clear();
				best_dirs.push_back(dir);
			}
			else if (free_slots == max_free_slots) best_dirs.push_back(dir);
		}
		else if (free_slots >= router[dir][flit.vc_id].GetCapacity() - 0) best_dirs.push_back(dir);
	}

	if (best_dirs.size() > 0) return best_dirs[rand() % best_dirs.size()];
	else return -1;
}
