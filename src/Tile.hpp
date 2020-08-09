/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the tile
 */

#pragma once
#include <systemc.h>
#include "Router.hpp"
#include "Processor.hpp"



class Tile : public sc_module
{
private:
	sc_signal<int> free_slots_local;
	sc_signal<int> free_slots_neighbor_local;

	// Signals required for Router-PE connection
	sc_signal<Flit> flit_rx_local;
	sc_signal<bool> req_rx_local;
	sc_signal<bool> ack_rx_local;
	sc_signal<bool> buffer_full_status_rx_local;

	sc_signal<Flit> flit_tx_local;
	sc_signal<bool> req_tx_local;
	sc_signal<bool> ack_tx_local;
	sc_signal<bool> buffer_full_status_tx_local;

public:
	struct Relay : sc_module
	{
		sc_in<Flit>					rx_flit;				// The input channels
		sc_in<bool>					rx_req;					// The requests associated with the input channels
		sc_out<bool>				rx_ack;					// The outgoing ack signals associated with the input channels
		sc_out<bool>	rx_buffer_full_status;
		sc_out<int>					free_slots;

		sc_out<Flit>				tx_flit;				// The output channels
		sc_out<bool>				tx_req;					// The requests associated with the output channels
		sc_in<bool>					tx_ack;					// The outgoing ack signals associated with the output channels
		sc_in<bool>	tx_buffer_full_status;
		sc_in<int>					free_slots_neighbor;

		sc_signal<Flit>				sig_flit;				// The input channels
		sc_signal<bool>				sig_req;				// The requests associated with the input channels
		sc_signal<bool>				sig_ack;				// The outgoing ack signals associated with the input channels
		sc_signal<bool>sig_buffer_full_status;
		sc_signal<int>				sig_free_slots;

		Relay(sc_module_name)
		{
			rx_flit(sig_flit);
			rx_req(sig_req);
			rx_ack(sig_ack);
			rx_buffer_full_status(sig_buffer_full_status);
			free_slots(sig_free_slots);
		}
		void Bind(Relay& r)
		{
			tx_flit(r.sig_flit);
			tx_req(r.sig_req);
			tx_ack(r.sig_ack);
			tx_buffer_full_status(r.sig_buffer_full_status);
			free_slots_neighbor(r.sig_free_slots);
			if (r.tx_flit.bind_count() == 0)
			{
				r.tx_flit(sig_flit);
				r.tx_req(sig_req);
				r.tx_ack(sig_ack);
				r.tx_buffer_full_status(sig_buffer_full_status);
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

	sc_vector<Relay> Relays;

	// I/O Ports
	sc_in_clk clock;							// The input clock for the tile
	sc_in<bool> reset;	                        // The reset signal for the tile

	// Instances
	std::unique_ptr<Router> RouterDevice;
	std::unique_ptr<Processor> ProcessorDevice;
		
	// Constructor
	Tile(sc_module_name nm, size_t relays) : 
		sc_module(nm), Relays("TileRelays", relays)
	{
	}
	void SetRouter(std::unique_ptr<Router>& router)
	{
		if (!router) throw std::runtime_error("Tile error: Router can not be null.");
		if (router->Relays.size() != Relays.size() + 1)
			throw std::runtime_error("Tile error: Incompatible numbers of relays.");

		RouterDevice = std::move(router);
		// Router pin assignments
		RouterDevice->clock(clock);
		RouterDevice->reset(reset);

		for (size_t i = 0; i < Relays.size(); i++)
		{
			RouterDevice->Relays[i].rx_flit(Relays[i].rx_flit);
			RouterDevice->Relays[i].rx_req(Relays[i].rx_req);
			RouterDevice->Relays[i].rx_ack(Relays[i].rx_ack);
			RouterDevice->Relays[i].rx_buffer_full_status(Relays[i].rx_buffer_full_status);

			RouterDevice->Relays[i].tx_flit(Relays[i].tx_flit);
			RouterDevice->Relays[i].tx_req(Relays[i].tx_req);
			RouterDevice->Relays[i].tx_ack(Relays[i].tx_ack);
			RouterDevice->Relays[i].tx_buffer_full_status(Relays[i].tx_buffer_full_status);

			RouterDevice->Relays[i].free_slots(Relays[i].free_slots);
			RouterDevice->Relays[i].free_slots_neighbor(Relays[i].free_slots_neighbor);
		}

		// local
		RouterDevice->LocalRelay.rx_flit(flit_tx_local);
		RouterDevice->LocalRelay.rx_req(req_tx_local);
		RouterDevice->LocalRelay.rx_ack(ack_tx_local);
		RouterDevice->LocalRelay.rx_buffer_full_status(buffer_full_status_tx_local);

		RouterDevice->LocalRelay.tx_flit(flit_rx_local);
		RouterDevice->LocalRelay.tx_req(req_rx_local);
		RouterDevice->LocalRelay.tx_ack(ack_rx_local);
		RouterDevice->LocalRelay.tx_buffer_full_status(buffer_full_status_rx_local);

		// NoP
		RouterDevice->LocalRelay.free_slots(free_slots_local);
		RouterDevice->LocalRelay.free_slots_neighbor(free_slots_neighbor_local);
	}
	void SetProcessor(std::unique_ptr<Processor>& processor)
	{
		if (!processor) throw std::runtime_error("Tile error: Processor can not be null.");

		ProcessorDevice = std::move(processor);

		// Processing Element pin assignments
		ProcessorDevice->clock(clock);
		ProcessorDevice->reset(reset);

		ProcessorDevice->flit_rx(flit_rx_local);
		ProcessorDevice->req_rx(req_rx_local);
		ProcessorDevice->ack_rx(ack_rx_local);
		ProcessorDevice->buffer_full_status_rx(buffer_full_status_rx_local);

		ProcessorDevice->flit_tx(flit_tx_local);
		ProcessorDevice->req_tx(req_tx_local);
		ProcessorDevice->ack_tx(ack_tx_local);
		ProcessorDevice->buffer_full_status_tx(buffer_full_status_tx_local);
		ProcessorDevice->free_slots_neighbor(free_slots_neighbor_local);
	}

	void ConfigureRotuerPower(const Configuration::Power& config, int32_t flit_size, int32_t buffer_depth, const std::string& routing_algorithm)
	{
		RouterDevice->power.configureRouter(config, 
			flit_size, buffer_depth, flit_size,
			routing_algorithm, "default");
	}
};
