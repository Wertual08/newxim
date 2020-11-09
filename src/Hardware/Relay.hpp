#pragma once
#include <systemc.h>
#include "DataStructs.hpp"
#include "Buffer.hpp"



class Relay
{
private:
	sc_in<Flit>					rx_flit;				// The input channels 
	sc_out<Flit>				tx_flit;				// The output channels
	sc_signal<Flit>				sig_flit;				// The input channels

	sc_in<bool>					rx_req;					// The requests associated with the input channels
	sc_out<bool>				tx_req;					// The requests associated with the output channels
	sc_signal<bool>				sig_req;				// The requests associated with the input channels

	sc_out<bool>				rx_ack;					// The outgoing ack signals associated with the input channels
	sc_in<bool>					tx_ack;					// The outgoing ack signals associated with the output channels
	sc_signal<bool>				sig_ack;				// The outgoing ack signals associated with the input channels


	bool rx_current_level = false;						// Current level for Alternating Bit Protocol (ABP)
	bool tx_current_level = false;						// Current level for Alternating Bit Protocol (ABP)
	
	std::size_t num_virtual_channels = 0;
	std::size_t current_virtual_channel = 0;
    sc_in<std::size_t>*		rx_free_slots = nullptr;
    sc_out<std::size_t>*		tx_free_slots = nullptr;
	sc_signal<std::size_t>*	sig_free_slots = nullptr;
	Buffer* buffers = nullptr;							// buffers[virtual_channel] 

public:
	Relay()
	{
		rx_flit(sig_flit);
		rx_req(sig_req);
		rx_ack(sig_ack);
	}
	~Relay()
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

	void SetVirtualChannels(std::size_t vcs)
	{
		num_virtual_channels = vcs;
		current_virtual_channel = 0;

		delete[] sig_free_slots;
		delete[] rx_free_slots;
		delete[] tx_free_slots;
		delete[] buffers;

		rx_free_slots = new sc_in<std::size_t>[vcs];
		tx_free_slots = new sc_out<std::size_t>[vcs];
		sig_free_slots = new sc_signal<std::size_t>[vcs];

		for (std::size_t i = 0; i < num_virtual_channels; i++)
			rx_free_slots[i](sig_free_slots[i]);

		buffers = new Buffer[vcs];
	}
	std::size_t Size() const { return num_virtual_channels; }
	void Bind(Relay& r)
	{
		tx_flit(r.sig_flit);
		tx_req(r.sig_req);
		tx_ack(r.sig_ack);
		for (std::size_t i = 0; i < num_virtual_channels; i++)
			tx_free_slots[i](r.sig_free_slots[i]);
		if (r.tx_flit.bind_count() == 0)
		{
			r.tx_flit(sig_flit);
			r.tx_req(sig_req);
			r.tx_ack(sig_ack);
			for (std::size_t i = 0; i < num_virtual_channels; i++)
				r.tx_free_slots[i](sig_free_slots[i]);
		}
	}
	bool Bound()
	{
		return tx_flit.bind_count();
	}
	void Disable()
	{
		Bind(*this);
		tx_req.write(false);
		rx_ack.write(false);
		for (std::size_t i = 0; i < num_virtual_channels; i++)
			tx_free_slots[i].write(-1);
	}

	void Reset()
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
	bool CanSend(const Flit& flit)
	{
		return tx_current_level == tx_ack.read() && rx_free_slots[flit.vc_id].read() > 0;
	}
	bool Send(const Flit& flit)
	{
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
	bool CanReceive()
	{
		return rx_req.read() == !rx_current_level;
	}
	bool Receive()
	{
		if (rx_req.read() == !rx_current_level)
		{
			Flit received_flit = rx_flit.read();
			received_flit.hop_no++;
			auto& buffer = buffers[received_flit.vc_id];

			if (buffer.Full()) throw std::runtime_error("Relay error: Buffer overflow.");
			
			// Store the incoming flit in the circular buffer
			buffer.Push(received_flit);

			// Negate the old value for Alternating Bit Protocol (ABP)
			rx_current_level = !rx_current_level;
			rx_ack.write(rx_current_level);

			return true;
		}
		else return false;
	}
	void UpdateFreeSlots()
	{
		for (std::size_t vc = 0; vc < num_virtual_channels; vc++)
			tx_free_slots[vc].write(buffers[vc].GetFreeSlots());
	}
	std::size_t GetFreeSlots(std::size_t vc) const
	{
		return rx_free_slots[vc].read();
	}

	bool FlitAvailable()
	{
		std::size_t i = 0;
		while (i < num_virtual_channels && buffers[(current_virtual_channel + i) % num_virtual_channels].Empty()) i++;
		return i < num_virtual_channels;
	}
	Flit Front()
	{
		std::size_t i = 0;
		while (i < num_virtual_channels && buffers[(current_virtual_channel + i) % num_virtual_channels].Empty()) i++;

		if (i < num_virtual_channels) return buffers[(current_virtual_channel + i) % num_virtual_channels].Front();
		else return Flit();
	}
	void Skip()
	{
		current_virtual_channel = (current_virtual_channel + 1) % num_virtual_channels;
	}
	Flit Pop()
	{
		std::size_t vc_offset = current_virtual_channel;
		current_virtual_channel = (current_virtual_channel + 1) % num_virtual_channels;
		
		std::size_t i = 0;
		while (i < num_virtual_channels && buffers[(vc_offset + i) % num_virtual_channels].Empty()) i++;

		if (i < num_virtual_channels) return buffers[(vc_offset + i) % num_virtual_channels].Pop();
		else return Flit();
	}

	Buffer& operator[](std::size_t i) { return buffers[i]; }
	const Buffer& operator[](std::size_t i) const { return buffers[i]; }
};