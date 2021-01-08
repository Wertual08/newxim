#pragma once
#include <systemc.h>
#include "Hardware/Router.hpp"
#include "Processor.hpp"



class Tile
{
public:
	// I/O Ports
	sc_in_clk clock;							// The input clock for the tile
	sc_in<bool> reset;	                        // The reset signal for the tile

	// Instances
	std::unique_ptr<Router> RouterDevice;
	std::unique_ptr<Processor> ProcessorDevice;

	Tile() {}

	void SetRouter(std::unique_ptr<Router>& router)
	{
		if (!router) throw std::runtime_error("Tile error: Router can not be null.");

		RouterDevice = std::move(router);
		// Router pin assignments
		RouterDevice->clock(clock);
		RouterDevice->reset(reset);
	}
	void SetProcessor(std::unique_ptr<Processor>& processor)
	{
		if (!processor) throw std::runtime_error("Tile error: Processor can not be null.");

		ProcessorDevice = std::move(processor);

		// Processing Element pin assignments
		ProcessorDevice->clock(clock);
		ProcessorDevice->reset(reset);
	}
};
