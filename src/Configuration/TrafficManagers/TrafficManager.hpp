#pragma once
#include <vector>



class TrafficManager
{
public:
	virtual bool FirePacket(std::int32_t from, double time) const = 0;
	virtual std::int32_t FindDestination(std::int32_t from) const = 0;
};