#include "Router.hpp"
#include <iomanip>
#include "Routing/RoutingAlgorithm.hpp"
#include "Selection/SelectionStrategy.hpp"



static std::string GetRouterName(std::int32_t id)
{
	return (std::stringstream() << "Router[" << std::setfill('0') << std::setw(3) << id << "]").str();
}


Router::Router(sc_module_name, const SimulationTimer& timer, std::int32_t id, std::size_t size) :
	relays(size + 1),
	stats(timer),
	LocalId(id),
	LocalRelay(relays[size]),
	LocalRelayId(size),
	routing(nullptr),
	selection(nullptr)
{
	for (std::int32_t i = 0; i < relays.size(); i++) {
		relays[i].SetLocalID(i);
	}

	SC_METHOD(Update);
	sensitive << reset << clock.pos();

	start_from_port = LocalRelayId;
}

void Router::Reservation(std::int32_t in_port)
{
	Relay &rel = relays[in_port];

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
		for (auto& relay : relays) relay.Reset();

		return;
	}

	TXProcess();
	RXProcess();

	start_from_port = (start_from_port + 1) % relays.size();

	for (int i = 0; i < relays.size(); i++)
	{
		relays[i].Update();
		for (int j = 0; j < relays[i].Size(); j++)
			stats.PushLoad(i, j, relays[i][j].GetLoad());
	}
}

Connection Router::FindDestination(const Flit& flit) 
{
	if (flit.dst_id == LocalId) return { LocalRelayId, 0 };

	routing_buffer.clear(); 
	routing->Route(*this, flit, routing_buffer);
	return selection->Apply(*this, flit, routing_buffer);
}
bool Router::Route(std::int32_t in_port, Connection dst)
{
	Relay& in_relay = relays[in_port];
	Relay& out_relay = relays[dst.port];

	Flit flit = in_relay.Front();
	std::int32_t in_vc = flit.vc_id;
	flit.vc_id = dst.vc;

	if (out_relay.Send(flit))
	{
		in_relay.Pop();

		// --------------- Stats --------------- //
		stats.FlitRouted(flit);
		stats.StopStuckTimer(in_port, in_vc);
		if (in_relay[flit.vc_id].Size()) {
			stats.StartStuckTimer(in_port, in_vc);
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
	for (std::size_t i = 0; i < relays.size(); i++)
	{
		if (relays[i].CanReceive())
		{
			Flit flit = relays[i].Receive();

			// --------------- Stats --------------- //
			stats.FlitReceived(i, flit.vc_id);
			stats.StartStuckTimer(i, flit.vc_id);
			if (tracer) {
				tracer->Remember(flit, LocalId);
			}
			// --------------- ----- --------------- //
		}
	}
}

void Router::TXProcess()
{
	// Reservation
	if (update_sequence.empty()) {
		for (std::int32_t j = 0; j < relays.size(); j++) {
			std::int32_t in_port = (start_from_port + j) % relays.size();
			Reservation(in_port);
		}
	} else {
		for (std::int32_t in_port : update_sequence) {
			if (in_port < relays.size()) {
				Reservation(in_port);
			}
		}
	}

	// Forwarding
	for (std::int32_t in_port = 0; in_port < relays.size(); in_port++) {
		Relay& relay = relays[in_port];

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
	
	for (auto& relay : relays) {
		relay.Skip();
	}
}

Router::Router(const SimulationTimer& timer, std::int32_t id, std::size_t size) :
	Router(GetRouterName(id).c_str(), timer, id, size)
{
}
void Router::SetRoutingAlgorithm(const RoutingAlgorithm& alg)
{
	routing = &alg;
}
void Router::SetSelectionStrategy(const SelectionStrategy& sel)
{
	selection = &sel;
}
void Router::SetFlitTracer(FlitTracer& tracer)
{
	this->tracer = &tracer;
}
void Router::SetUpdateSequence(const std::vector<std::int32_t> &sequence)
{
	update_sequence = sequence;
}

std::size_t Router::TotalBufferedFlits() const
{
	std::size_t count = 0;
	for (std::size_t i = 0; i < relays.size(); i++)
	{
		for (std::size_t vc = 0; vc < relays[i].Size(); vc++)
		{
			count += relays[i][vc].Size();
		}
	}
	return count;
}
std::size_t Router::DestinationFreeSlots(Connection dst) const
{
	return relays[dst.port].GetFreeSlots(dst.vc);
}
bool Router::CanSend(Connection dst) const
{
	return 
		!reservation_table.Reserved(dst) &&
		relays[dst.port].CanSend(dst.vc);
}
