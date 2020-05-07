#pragma once
#include "SelectionStrategy.h"
#include "../Router.h"



class SelectionTorusAdaptive : public SelectionStrategy
{
private:
	const int32_t TorusW, TorusH;

	int32_t GetXFromID(int32_t id);
	int32_t GetYFromID(int32_t id);

public:
	SelectionTorusAdaptive(int32_t w, int32_t h);

	int32_t Apply(Router& router, const std::vector<int32_t>& directions, const RouteData& route_data) override;
	void PerCycleProcess(Router& router) override;
};
