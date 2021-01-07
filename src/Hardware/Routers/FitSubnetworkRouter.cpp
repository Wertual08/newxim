#include "FitSubnetworkRouter.hpp"



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

void FitSubnetworkRouter::TXProcess()
{
	for (std::int32_t j = 0; j < Relays.size(); j++)
	{
		std::int32_t in_port = (start_from_port + j) % static_cast<std::int32_t>(Relays.size());
		Relay& rel = Relays[in_port];

		if (rel.FlitAvailable())
		{
			Flit flit = rel.Front();
			
			power.bufferRouterFront();

			if (HasFlag(flit.flit_type, FlitType::Head))
			{
				Connection src = { in_port, flit.vc_id };
				Connection dst = FindDestination(flit); 
				if (!dst.valid()) continue;

				// TODO: Maybe it should select one of the channels... (Apply selection strategy???)
				if (DestinationFreeSlots(dst) < flit.sequence_length)
					dst.port = SubnetworkTable[flit.dst_id][0];
				if (DestinationFreeSlots(dst) < flit.sequence_length)
					continue;

				if (!reservation_table.Reserved(src, dst))
					reservation_table.Reserve(src, dst);
			}
		}
	}

	for (std::int32_t in_port = 0; in_port < Relays.size(); in_port++)
	{
		Relay& rel = Relays[in_port];

		if (rel.FlitAvailable())
		{
			Flit flit = rel.Front();

			Connection src = { in_port, flit.vc_id };
			Connection dst = reservation_table[src];

			if (!dst.valid()) continue;

			if (Route(in_port, dst) && HasFlag(flit.flit_type, FlitType::Tail))
				reservation_table.Release(src);
		}
	}

	for (auto& relay : Relays) relay.Skip();
}
