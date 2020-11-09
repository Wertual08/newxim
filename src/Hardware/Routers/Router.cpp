#include "Router.hpp"



static std::string GetRouterName(std::int32_t id)
{
	return (std::stringstream() << "Router[" << std::setfill('0') << std::setw(3) << id << "]").str();
}


Router::Router(sc_module_name, const SimulationTimer& timer, std::int32_t id, std::size_t relays) :
	Relays(relays + 1),
	stats(timer),
	power(timer),
	LocalID(id),
	LocalRelay(Relays[relays]),
	LocalRelayID(relays),
	Routing(nullptr),
	Selection(nullptr)
{
	SC_METHOD(Update);
	sensitive << reset << clock.pos();

	start_from_port = LocalRelayID;//rand() % Relays.size();
}

void Router::Update()
{
	if (reset.read())
	{
		for (std::size_t i = 0; i < Relays.size(); i++)
			Relays[i].Reset();

		return;
	}

	TXProcess();
	RXProcess();

	start_from_port = (start_from_port + 1) % Relays.size();

	power.leakageRouter();
	for (int i = 0; i < Relays.size(); i++)
	{
		power.leakageBufferRouter();
		power.leakageLinkRouter2Router();
		for (int j = 0; j < Relays[i].Size(); j++)
			stats.PushLoad(i, j, Relays[i][j].GetLoad());
	}
}

std::int32_t Router::PerformRoute(Flit& flit)
{
	if (flit.dst_id == LocalID) return LocalRelayID;

	power.routing();
	power.selection();
	return Selection->Apply(*this, flit, Routing->Route(*this, flit));
}
bool Router::Route(std::int32_t in_port, std::int32_t out_port, std::int32_t vc)
{
	Relay& in_relay = Relays[in_port];
	Relay& out_relay = Relays[out_port];

	Flit flit = in_relay.Front();
	flit.dir_in = in_port;
	flit.vc_id = vc;

	if (out_relay.Send(flit))
	{
		flit = in_relay.Pop();
		flit.dir_in = in_port;

		/* Power & Stats ------------------------------------------------- */
		stats.StopStuckTimer(in_port, flit.vc_id);
		if (in_relay[flit.vc_id].Size()) stats.StartStuckTimer(in_port, flit.vc_id);

		power.r2rLink();

		power.bufferRouterPop();
		power.crossBar();

		if (out_port == LocalRelayID)
		{
			power.networkInterface();
		}
		/* End Power & Stats ------------------------------------------------- */

		return true;
	}
	else return false;
}

void Router::RXProcess()
{
	// This process simply sees a flow of incoming flits. All arbitration
	// and wormhole related issues are addressed in the txProcess()
	for (std::size_t i = 0; i < Relays.size(); i++)
	{
		if (Relays[i].Receive())
		{
			power.bufferRouterPush();
			// if a new flit is injected from local PE
			if (i == LocalRelayID)
			{
				power.networkInterface();
			}
		}

		Relays[i].UpdateFreeSlots();
	}
}

Router::Router(const SimulationTimer& timer, std::int32_t id, std::size_t relays) :
	Router(GetRouterName(id).c_str(), timer, id, relays)
{
}
void Router::SetRoutingAlgorithm(const RoutingAlgorithm& alg)
{
	Routing = &alg;
}
void Router::SetSelectionStrategy(const SelectionStrategy& sel)
{
	Selection = &sel;
}

std::size_t Router::TotalBufferedFlits() const
{
	std::size_t count = 0;
	for (std::size_t i = 0; i < Relays.size(); i++)
	{
		for (std::size_t vc = 0; vc < Relays[i].Size(); vc++)
		{
			count += Relays[i][vc].Size();
		}
	}
	return count;
}
