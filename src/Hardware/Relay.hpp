#pragma once
#include <systemc.h>
#include "DataStructs.hpp"
#include "Buffer.hpp"



class Relay
{
private:
	sc_signal<Flit>				sig_flit;				// The input channels
	sc_signal<bool>				sig_req;				// The requests associated with the input channels
	sc_signal<bool>				sig_ack;				// The outgoing ack signals associated with the input channels
	sc_signal<bool>				sig_buffer_full_status;
	sc_signal<std::int32_t>		sig_free_slots;

public:
	sc_in<Flit>					rx_flit;				// The input channels 
	sc_in<bool>					rx_req;					// The requests associated with the input channels
	sc_out<bool>				rx_ack;					// The outgoing ack signals associated with the input channels

	sc_out<Flit>				tx_flit;				// The output channels
	sc_out<bool>				tx_req;					// The requests associated with the output channels
	sc_in<bool>					tx_ack;					// The outgoing ack signals associated with the output channels

	sc_out<std::int32_t>		free_slots;
	sc_in<std::int32_t>			free_slots_neighbor;

	Buffer buffer;										// buffer[direction][virtual_channel] 
	bool rx_current_level;								// Current level for Alternating Bit Protocol (ABP)
	bool tx_current_level;								// Current level for Alternating Bit Protocol (ABP)
	
	Relay()
	{
		rx_flit(sig_flit);
		rx_req(sig_req);
		rx_ack(sig_ack);
		free_slots(sig_free_slots);
		rx_current_level = false;
		tx_current_level = false;
	}
	void Bind(Relay& r)
	{
		tx_flit(r.sig_flit);
		tx_req(r.sig_req);
		tx_ack(r.sig_ack);
		free_slots_neighbor(r.sig_free_slots);
		if (r.tx_flit.bind_count() == 0)
		{
			r.tx_flit(sig_flit);
			r.tx_req(sig_req);
			r.tx_ack(sig_ack);
			r.free_slots_neighbor(sig_free_slots);
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
		free_slots.write(-1);
	}
};