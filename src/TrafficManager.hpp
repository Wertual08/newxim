#pragma once
#include <vector>



class TrafficManager
{
public:
	struct Load
	{
		std::int32_t SendFactor = 1;
		std::int32_t ReceiveFactor = 1;
	};

private:
	std::vector<Load> TrafficLoad;
	std::vector<std::int32_t> Destinations;

public:
	void Init(std::int32_t count);
	void SetLoad(std::int32_t node, std::int32_t send_factor, std::int32_t receive_factor);
	void Setup();

	bool FirePacket(std::int32_t from, double threshold) const;
	std::int32_t FindDestination(std::int32_t from) const;
};