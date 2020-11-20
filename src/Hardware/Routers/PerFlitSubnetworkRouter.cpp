#include "PerFlitSubnetworkRouter.hpp"



static bool VectorContains(const std::vector<std::int32_t>& vec, std::int32_t v)
{
	return std::find(vec.begin(), vec.end(), v) != vec.end();
}
static bool VectorContains(const std::vector<std::vector<std::int32_t>>& vec, std::int32_t v)
{
	for (const auto& sub_vec : vec)
	{
		if (std::find(sub_vec.begin(), sub_vec.end(), v) != sub_vec.end())
			return true;
	}
	return false;
}

void PerFlitSubnetworkRouter::TXProcess()
{
	for (std::int32_t j = 0; j < Relays.size(); j++)
	{
		std::int32_t in_port = (start_from_port + j) % static_cast<std::int32_t>(Relays.size());
		Relay& in_relay = Relays[in_port];

		if (in_relay.FlitAvailable())
		{
			Flit flit = in_relay.Front();
			Connection src = { in_port, flit.vc_id };
			Connection dst = FindDestination(flit);
			
			if (!dst.valid()) continue;

			// TODO: Maybe it should select one of the channels... (Apply selection strategy???)
			if (Relays[dst.port].GetFreeSlots(dst.vc) < 1)
				dst.port = SubnetworkTable[flit.dst_id][0];

			Route(in_port, dst);
		}
	} // for loop directions

	for (auto& relay : Relays) relay.Skip();
}

PerFlitSubnetworkRouter::PerFlitSubnetworkRouter(const SimulationTimer& timer, std::int32_t id, std::size_t relays) :
	SubnetworkRouter(timer, id, relays)
{
}
