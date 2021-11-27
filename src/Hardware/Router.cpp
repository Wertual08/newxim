#include "Router.hpp"
#include <iomanip>
#include "Routing/RoutingAlgorithm.hpp"
#include "Selection/SelectionStrategy.hpp"



static std::string GetRouterName(std::int32_t id)
{
	return (std::stringstream() << "Router[" << std::setfill('0') << std::setw(3) << id << "]").str();
}


Router::Router(sc_module_name, const SimulationTimer& timer, std::int32_t id, std::size_t relays) :
	Relays(relays + 1),
	stats(timer),
	LocalID(id),
	LocalRelay(Relays[relays]),
	LocalRelayID(relays),
	Routing(nullptr),
	Selection(nullptr)
{
	for (std::int32_t i = 0; i < Relays.size(); i++) {
		Relays[i].SetLocalID(i);
	}

	SC_METHOD(Update);
	sensitive << reset << clock.pos();

	start_from_port = LocalRelayID;
}

void Router::Reservation(std::int32_t in_port)
{
	Relay &rel = Relays[in_port];

	Flit flit = rel.Front();
	if (flit.valid() && HasFlag(flit.flit_type, FlitType::Head)) {
		Connection src = { in_port, flit.vc_id };
		Connection dst = FindDestination(flit);
		if (dst.valid() && !reservation_table.Reserved(src, dst)) {
			reservation_table.Reserve(src, dst);
		}
	}
}
void Router::Update()
{
	if (reset.read())
	{
		for (auto& relay : Relays) relay.Reset();

		return;
	}

	TXProcess();
	RXProcess();

	start_from_port = (start_from_port + 1) % Relays.size();

	for (int i = 0; i < Relays.size(); i++)
	{
		Relays[i].Update();
		for (int j = 0; j < Relays[i].Size(); j++)
			stats.PushLoad(i, j, Relays[i][j].GetLoad());
	}
}

Connection Router::FindDestination(const Flit& flit) 
{
	if (flit.dst_id == LocalID) return { LocalRelayID, 0 };

	routing_buffer.clear(); 
	Routing->Route(*this, flit, routing_buffer);
	return Selection->Apply(*this, flit, routing_buffer);
}
bool Router::Route(std::int32_t in_port, Connection dst)
{
	Relay& in_relay = Relays[in_port];
	Relay& out_relay = Relays[dst.port];

	Flit flit = in_relay.Front();
	flit.vc_id = dst.vc;

	if (out_relay.Send(flit))
	{
		in_relay.Pop();

		// --------------- Stats --------------- //
		stats.FlitRouted();
		stats.StopStuckTimer(in_port, flit.vc_id);
		if (in_relay[flit.vc_id].Size()) {
			stats.StartStuckTimer(in_port, flit.vc_id);
		}
		// --------------- ----- --------------- //

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
		if (Relays[i].CanReceive())
		{
			Flit flit = Relays[i].Receive();

			// --------------- Stats --------------- //
			stats.StartStuckTimer(i, flit.vc_id);
			if (Tracer) {
				Tracer->Remember(flit, LocalID);
			}
			// --------------- ----- --------------- //
		}
	}
}

void Router::TXProcess()
{
	// 1st phase: Reservation
	if (update_sequence.empty()) {
		for (std::int32_t j = 0; j < Relays.size(); j++) {
			std::int32_t in_port = (start_from_port + j) % Relays.size();
			Reservation(in_port);
		}
	} else {
		for (std::int32_t in_port : update_sequence) {
			if (in_port < Relays.size()) {
				Reservation(in_port);
			}
		}
	}

	// 2nd phase: Forwarding
	for (std::int32_t in_port = 0; in_port < Relays.size(); in_port++) {
		Relay& relay = Relays[in_port];

		Flit flit = relay.Front();
		if (flit.valid()) {
			Connection src = { in_port, flit.vc_id };
			Connection dst = reservation_table[src];

			if (!dst.valid()) {
				continue;
			}

			if (Route(in_port, dst) && HasFlag(flit.flit_type, FlitType::Tail)) {
				reservation_table.Release(src);
			}
		}

	}
	
	for (auto& relay : Relays) {
		relay.Skip();
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
void Router::SetFlitTracer(FlitTracer& tracer)
{
	Tracer = &tracer;
}
void Router::SetUpdateSequence(const std::vector<std::int32_t> &sequence)
{
	update_sequence = sequence;
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
std::size_t Router::DestinationFreeSlots(Connection dst) const
{
	return Relays[dst.port].GetFreeSlots(dst.vc);
}
bool Router::CanSend(Connection dst) const
{
	return 
		!reservation_table.Reserved(dst) &&
		Relays[dst.port].CanSend(dst.vc);
}
