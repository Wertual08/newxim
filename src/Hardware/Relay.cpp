#include "Relay.hpp"



Relay::Relay()
{
	rx_flit(sig_flit);
	rx_req(sig_req);
	rx_ack(sig_ack);
}
Relay::~Relay()
{
	delete[] sig_free_slots;
	delete[] rx_free_slots;
	delete[] tx_free_slots;
	delete[] buffers;

	sig_free_slots = nullptr;
	rx_free_slots = nullptr;
	tx_free_slots = nullptr;
	buffers = nullptr;
}

void Relay::SetLocalID(std::int32_t id)
{
	local_id = id;
}
void Relay::SetVirtualChannels(std::size_t vcs)
{
	num_virtual_channels = vcs;
	current_virtual_channel = 0;

	delete[] sig_free_slots;
	delete[] rx_free_slots;
	delete[] buffers;

	rx_free_slots = new sc_in<std::size_t>[vcs];
	sig_free_slots = new sc_signal<std::size_t>[vcs];

	for (std::size_t i = 0; i < num_virtual_channels; i++)
		rx_free_slots[i](sig_free_slots[i]);

	buffers = new Buffer[vcs];
}
void Relay::Bind(Relay& r)
{
	tx_flit(r.sig_flit);
	tx_req(r.sig_req);
	tx_ack(r.sig_ack);
	
	delete[] tx_free_slots;
	tx_free_slots = new sc_out<std::size_t>[r.num_virtual_channels];
	for (std::size_t i = 0; i < r.num_virtual_channels; i++)
		tx_free_slots[i](r.sig_free_slots[i]);

	bound = &r;

	if (!r.bound)
	{
		r.tx_flit(sig_flit);
		r.tx_req(sig_req);
		r.tx_ack(sig_ack);
		delete[] r.tx_free_slots;
		r.tx_free_slots = new sc_out<std::size_t>[num_virtual_channels];
		for (std::size_t i = 0; i < num_virtual_channels; i++)
			r.tx_free_slots[i](sig_free_slots[i]);
		r.bound = this;
	}
}
void Relay::Disable()
{
	Bind(*this);
	tx_req.write(false);
	rx_ack.write(false);
	for (std::size_t i = 0; i < num_virtual_channels; i++)
		tx_free_slots[i].write(-1);
}

void Relay::Reset()
{
	// Clear out
	tx_req.write(0);
	tx_current_level = false;

	// Clear outputs and indexes of receiving protocol
	rx_ack.write(0);
	rx_current_level = false;
	for (std::size_t i = 0; i < num_virtual_channels; i++)
	{
		buffers[i].Clear();
		tx_free_slots[i].write(buffers[i].GetCapacity());
	}
}
void Relay::Update()
{
	rx_ack.write(rx_current_level);
	for (std::size_t i = 0; i < num_virtual_channels; i++)
		tx_free_slots[i].write(buffers[i].GetFreeSlots());
}
bool Relay::CanSend(const Flit& flit) const
{
	return tx_current_level == tx_ack.read() && rx_free_slots[flit.vc_id].read() > 0;
}
bool Relay::Send(const Flit& flit)
{
	if (tx_current_level != tx_ack.read())
		return false;

	if (tx_current_level == tx_ack.read() &&
		rx_free_slots[flit.vc_id].read() > 0)
	{
		tx_flit.write(flit);
		tx_current_level = !tx_current_level;
		tx_req.write(tx_current_level);

		return true;
	}
	else return false;
}
bool Relay::CanReceive() const
{
	return rx_req.read() == !rx_current_level;
}
Flit Relay::Receive()
{
	if (rx_req.read() == !rx_current_level)
	{
		Flit flit = rx_flit.read();
		flit.hop_no++;
		flit.dir_in = local_id;

		if (flit.vc_id < 0 || flit.vc_id >= num_virtual_channels) throw std::runtime_error(
			"Relay error: Invalid virtual channel [" + std::to_string(flit.vc_id) + "] of received flit.");

		auto& buffer = buffers[flit.vc_id];

		if (buffer.Full()) throw std::runtime_error("Relay error: Buffer overflow.");

		// Store the incoming flit in the circular buffer
		buffer.Push(flit);

		// Negate the old value for Alternating Bit Protocol (ABP)
		rx_current_level = !rx_current_level;
		//tx_free_slots[flit.vc_id].write(buffer.GetFreeSlots());

		return flit;
	}
	else return Flit();
}

bool Relay::FlitAvailable() const
{
	std::size_t i = 0;
	while (i < num_virtual_channels && buffers[(current_virtual_channel + i) % num_virtual_channels].Empty()) i++;
	return i < num_virtual_channels;
}
Flit Relay::Front() const
{
	std::size_t i = 0;
	while (i < num_virtual_channels && buffers[(current_virtual_channel + i) % num_virtual_channels].Empty()) i++;

	if (i < num_virtual_channels) return buffers[(current_virtual_channel + i) % num_virtual_channels].Front();
	else return Flit();
}
void Relay::Skip()
{
	current_virtual_channel = (current_virtual_channel + 1) % num_virtual_channels;
}
Flit Relay::Pop()
{
	std::size_t vc_offset = current_virtual_channel;

	std::size_t i = 0;
	while (i < num_virtual_channels && buffers[(vc_offset + i) % num_virtual_channels].Empty()) i++;
	std::size_t vc = (vc_offset + i) % num_virtual_channels;

	if (i < num_virtual_channels)
	{
		tx_free_slots[vc].write(buffers[vc].GetFreeSlots() + 1);
		return buffers[vc].Pop();
	}
	else return Flit();
}
