#pragma once
#include <vector>



class TrafficManager
{
public:
	struct Load
	{
		int32_t SendFactor = 1;
		int32_t ReceiveFactor = 1;
	};

private:
	std::vector<Load> TrafficLoad;
	std::vector<int32_t> Destinations;

public:
	void Init(int32_t count);
	void SetLoad(int32_t node, int32_t send_factor, int32_t receive_factor);
	void Setup();

	bool FirePacket(int32_t from, double threshold) const;
	int32_t FindDestination(int32_t from) const;
};