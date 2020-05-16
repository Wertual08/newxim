#pragma once
#include "RoutingAlgorithm.h"
#include "../Configuration.h"



RoutingAlgorithm* FindTestRouting(const std::string& name, const Configuration& config);

class TEST_RoutingChannelNextIDGraterCurrent : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelNextIDGraterCurrent(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelPrevIDGraterCurrent : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelPrevIDGraterCurrent(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelPrevIDGraterNext : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelPrevIDGraterNext(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelNextIDGraterCurrentDirectionInverse : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelNextIDGraterCurrentDirectionInverse(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelSourceID : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelSourceID(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelDestinationID : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelDestinationID(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelInputLinkLength : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelInputLinkLength(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelHopsGrater0 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelHopsGrater0(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelHopsGrater1 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelHopsGrater1(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelHopsGrater2 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelHopsGrater2(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelHopsSwitchNotMod2 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelHopsSwitchNotMod2(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelHopsSwitchNotMod3 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelHopsSwitchNotMod3(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};

class TEST_RoutingChannelHopsSwitchNotMod4 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;

public:
	TEST_RoutingChannelHopsSwitchNotMod4(const Graph& graph);
	std::vector<int32_t> Route(Router& router, const RouteData& route_data) override;
};