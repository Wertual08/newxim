#include "SelectionRandomKeepSpace.h"
#include "../Router.h"



int32_t SelectionRandomKeepSpace::Apply(Router& router, const std::vector<int32_t>& directions, const RouteData& route_data)
{
	std::vector<int32_t> best_dirs;
	for (int32_t i = 0; i < directions.size(); i++)
	{
		int32_t dir = directions[i];
		int32_t free_slots = router.Relays[dir].free_slots_neighbor.read();

		if (route_data.dir_in != router.LocalRelayID) best_dirs.push_back(dir);
		else if (free_slots >= router.Relays[dir].buffer.GetMaxBufferSize() - 0) best_dirs.push_back(dir);
	}

	if (best_dirs.size() > 0) return best_dirs[rand() % best_dirs.size()];
	else return -1;
}

void SelectionRandomKeepSpace::PerCycleProcess(Router& router)
{
	// update current input buffers level to neighbors
	for (int32_t i = 0; i < router.Relays.size(); i++)
		router.Relays[i].free_slots.write(router.Relays[i].buffer.GetCurrentFreeSlots());
}
