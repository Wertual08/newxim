#pragma once
#include "RoutingAlgorithm.hpp"
#include "Configuration/Configuration.hpp"
#include "Configuration/RoutingTable.hpp"



std::unique_ptr<RoutingAlgorithm> FindTestRouting(const std::string& name, const Configuration& config, const RoutingTable& table);

class TEST_RoutingChannelNextIDGraterCurrent : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelNextIDGraterCurrent(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelPrevIDGraterCurrent : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelPrevIDGraterCurrent(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelPrevIDGraterNext : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelPrevIDGraterNext(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelNextIDGraterCurrentDirectionInverse : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelNextIDGraterCurrentDirectionInverse(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelSourceID : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelSourceID(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelDestinationID : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelDestinationID(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelInputLinkLength : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelInputLinkLength(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelHopsGrater0 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelHopsGrater0(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelHopsGrater1 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelHopsGrater1(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelHopsGrater2 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelHopsGrater2(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelHopsSwitchNotMod2 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelHopsSwitchNotMod2(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelHopsSwitchNotMod3 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelHopsSwitchNotMod3(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelHopsSwitchNotMod4 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelHopsSwitchNotMod4(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};

class TEST_RoutingChannelSwitchNode0AndNDiv2 : public RoutingAlgorithm
{
private:
	const Graph& TopologyGraph;
	const RoutingTable& Table;

public:
	TEST_RoutingChannelSwitchNode0AndNDiv2(const Graph& graph, const RoutingTable& table);
	std::vector<Connection> Route(const Router& router, const Flit& flit) const override;
};