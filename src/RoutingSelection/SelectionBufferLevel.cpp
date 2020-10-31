#include "SelectionBufferLevel.hpp"
#include "Hardware/Routers/Router.hpp"



std::int32_t SelectionBufferLevel::Apply(Router& router, const std::vector<std::int32_t>& directions, const RouteData& route_data) const
{
	std::vector<std::int32_t> best_dirs;
	std::int32_t max_free_slots = 0;
	for (std::int32_t i = 0; i < directions.size(); i++)
	{
		std::int32_t dir = directions[i];
		
		std::int32_t free_slots = router.Relays[dir].free_slots_neighbor.read();

		if (free_slots > max_free_slots) 
		{
			max_free_slots = free_slots;
			best_dirs.clear();
			best_dirs.push_back(dir);
		}
		else if (free_slots == max_free_slots) best_dirs.push_back(dir);
	}

	if (best_dirs.size()) return best_dirs[rand() % best_dirs.size()];
	else return directions[rand() % directions.size()];
}
