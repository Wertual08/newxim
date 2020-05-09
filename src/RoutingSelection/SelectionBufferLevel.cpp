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

		if (!router.reservation_table.Reserved(dir))
		{
			if (free_slots > max_free_slots) 
			{
				max_free_slots = free_slots;
				best_dirs.clear();
				best_dirs.push_back(dir);
			}
			else if (free_slots == max_free_slots) best_dirs.push_back(dir);
		}
	}

	if (best_dirs.size()) return best_dirs[rand() % best_dirs.size()];
	else return directions[rand() % directions.size()];

	//-------------------------
	// TODO: unfair if multiple directions have same buffer level
	// TODO: to check when both available
	//unsigned int max_free_slots = 0;
	//int direction_choosen = NOT_VALID;

	//for (unsigned int i=0;i<directions.size();i++)
	//  {
	//    int free_slots = free_slots_neighbor[directions[i]].read();
	//    if ((free_slots >= max_free_slots) &&
	//     (reservation_table.isNotReserved(directions[i])))
	//   {
	//     direction_choosen = directions[i];
	//     max_free_slots = free_slots;
	//   }
	//  }

	//// No available channel 
	//if (direction_choosen==NOT_VALID)
	//  direction_choosen = directions[rand() % directions.size()]; 

	//if(GlobalParams::verbose_mode>VERBOSE_OFF)
	//  {
	//    ChannelStatus tmp;

	//    LOG << "SELECTION between: " << endl;
	//    for (unsigned int i=0;i<directions.size();i++)
	//   {
	//     tmp.free_slots = free_slots_neighbor[directions[i]].read();
	//     tmp.available = (reservation_table.isNotReserved(directions[i]));
	//     cout << "    -> direction " << directions[i] << ", channel status: " << tmp << endl;
	//   }
	//    cout << " direction choosen: " << direction_choosen << endl;
	//  }

	//assert(direction_choosen>=0);
	//return direction_choosen;
}

void SelectionBufferLevel::PerCycleProcess(Router& router)
{
	// update current input buffers level to neighbors
	for (int i = 0; i < router.Relays.size(); i++)
		router.Relays[i].free_slots.write(router.Relays[i].buffer.GetCurrentFreeSlots());

	// NoP selection: send neighbor info to each direction 'i'
	//NoP_data current_NoP_data = router->getCurrentNoPData();
	//
	//for (int i = 0; i < DIRECTIONS; i++)
	//	router->NoP_data_out[i].write(current_NoP_data);
}
