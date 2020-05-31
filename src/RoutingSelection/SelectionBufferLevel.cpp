#include "SelectionBufferLevel.h"
#include "../Router.h"



int32_t SelectionBufferLevel::Apply(Router& router, const std::vector<int32_t>& directions, const RouteData& route_data)
{
	std::vector<int32_t> best_dirs;
	int32_t max_free_slots = 0;
	for (int32_t i = 0; i < directions.size(); i++)
	{
		int32_t dir = directions[i];
		
		int32_t free_slots = router.Relays[dir].free_slots_neighbor.read();

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

void SelectionBufferLevel::PerCycleProcess(Router& router)
{
	for (int i = 0; i < router.Relays.size(); i++)
		router.Relays[i].free_slots.write(router.Relays[i].buffer.GetCurrentFreeSlots());
}
