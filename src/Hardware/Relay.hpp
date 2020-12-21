#pragma once
#include "Utils.hpp"
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
	
	std::int32_t local_id = -1;
	std::size_t num_virtual_channels = 0;
	std::size_t current_virtual_channel = 0;
    sc_in<std::size_t>*		rx_free_slots = nullptr;
    sc_out<std::size_t>*	tx_free_slots = nullptr;
	sc_signal<std::size_t>*	sig_free_slots = nullptr;
	Buffer* buffers = nullptr;							// buffers[virtual_channel] 
	Relay* bound = nullptr;

public:
	Relay();
	~Relay();

	void SetLocalID(std::int32_t id);
	void SetVirtualChannels(std::size_t vcs);
	std::size_t Size() const { return num_virtual_channels; }
	void Bind(Relay& r);
	bool Bound() const { return bound; }
	void Disable();

	void Reset();
	void Update();
	bool CanSend(const Flit& flit) const;
	bool Send(const Flit& flit);
	bool CanReceive() const;
	Flit Receive();

	std::size_t GetFreeSlots(std::size_t vc) const 
	{ 
		return rx_free_slots[vc].read(); 
	}

	bool FlitAvailable() const;
	Flit Front() const;
	void Skip();
	Flit Pop();

	Buffer& operator[](std::size_t i) { return buffers[i]; }
	const Buffer& operator[](std::size_t i) const { return buffers[i]; }
};