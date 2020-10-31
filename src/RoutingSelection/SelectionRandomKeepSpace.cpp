#include "SelectionRandomKeepSpace.hpp"
#include "Hardware/Routers/Router.hpp"



std::int32_t SelectionRandomKeepSpace::Apply(Router& router, const std::vector<std::int32_t>& directions, const RouteData& route_data) const
{
	std::vector<std::int32_t> best_dirs;
	for (std::int32_t i = 0; i < directions.size(); i++)
	{
		std::int32_t dir = directions[i];
		std::int32_t free_slots = router.Relays[dir].free_slots_neighbor.read();

		if (route_data.dir_in != router.LocalRelayID) best_dirs.push_back(dir);
		else if (free_slots >= router.Relays[dir].buffer.GetMaxBufferSize() - 0) best_dirs.push_back(dir);
	}

	if (best_dirs.size() > 0) return best_dirs[rand() % best_dirs.size()];
	else return -1;
}
