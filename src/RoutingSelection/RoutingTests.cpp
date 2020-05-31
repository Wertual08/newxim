#include "RoutingTests.h"
#include "../Router.h"



std::unique_ptr<RoutingAlgorithm> FindTestRouting(const std::string& name, const Configuration& config, const RoutingTable& table)
{
    if (name == "TEST_RoutingChannelNextIDGraterCurrent") return std::make_unique<TEST_RoutingChannelNextIDGraterCurrent>(config.Topology(), table);
    if (name == "TEST_RoutingChannelPrevIDGraterCurrent") return std::make_unique<TEST_RoutingChannelPrevIDGraterCurrent>(config.Topology(), table);
    if (name == "TEST_RoutingChannelPrevIDGraterNext") return std::make_unique<TEST_RoutingChannelPrevIDGraterNext>(config.Topology(), table);
    if (name == "TEST_RoutingChannelNextIDGraterCurrentDirectionInverse") return std::make_unique<TEST_RoutingChannelNextIDGraterCurrentDirectionInverse>(config.Topology(), table);
    if (name == "TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse") return std::make_unique<TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse>(config.Topology(), table);
    if (name == "TEST_RoutingChannelSourceID") return std::make_unique<TEST_RoutingChannelSourceID>(config.Topology(), table);
    if (name == "TEST_RoutingChannelDestinationID") return std::make_unique<TEST_RoutingChannelDestinationID>(config.Topology(), table);
    if (name == "TEST_RoutingChannelInputLinkLength") return std::make_unique<TEST_RoutingChannelInputLinkLength>(config.Topology(), table);
    if (name == "TEST_RoutingChannelHopsGrater0") return std::make_unique<TEST_RoutingChannelHopsGrater0>(config.Topology(), table);
    if (name == "TEST_RoutingChannelHopsGrater1") return std::make_unique<TEST_RoutingChannelHopsGrater1>(config.Topology(), table);
    if (name == "TEST_RoutingChannelHopsGrater2") return std::make_unique<TEST_RoutingChannelHopsGrater2>(config.Topology(), table);
    if (name == "TEST_RoutingChannelHopsSwitchNotMod2") return std::make_unique<TEST_RoutingChannelHopsSwitchNotMod2>(config.Topology(), table);
    if (name == "TEST_RoutingChannelHopsSwitchNotMod3") return std::make_unique<TEST_RoutingChannelHopsSwitchNotMod3>(config.Topology(), table);
    if (name == "TEST_RoutingChannelHopsSwitchNotMod4") return std::make_unique<TEST_RoutingChannelHopsSwitchNotMod4>(config.Topology(), table);
	return nullptr;
}


TEST_RoutingChannelNextIDGraterCurrent::TEST_RoutingChannelNextIDGraterCurrent(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelNextIDGraterCurrent::Route(Router& router, const RouteData& route_data)  
{
    std::vector<int32_t> result;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t vc = 0;// prev_vc;
        int32_t next_id = TopologyGraph[route_data.current_id][r];

        if (next_id > route_data.current_id) vc = 1;
        else vc = 0;

        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelPrevIDGraterCurrent::TEST_RoutingChannelPrevIDGraterCurrent(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelPrevIDGraterCurrent::Route(Router& router, const RouteData& route_data)
{
    std::vector<int32_t> result;

    int32_t prev_id = route_data.current_id;
    if (route_data.dir_in < TopologyGraph[route_data.current_id].size())
        prev_id = TopologyGraph[route_data.current_id][route_data.dir_in];
   
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t vc = 0;
        int32_t next_id = TopologyGraph[route_data.current_id][r];

        if (prev_id > route_data.current_id) vc = 1;
        else vc = 0;

        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelPrevIDGraterNext::TEST_RoutingChannelPrevIDGraterNext(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelPrevIDGraterNext::Route(Router& router, const RouteData& route_data)
{
    std::vector<int32_t> result;

    int32_t prev_id = route_data.current_id;
    if (route_data.dir_in < TopologyGraph[route_data.current_id].size())
        prev_id = TopologyGraph[route_data.current_id][route_data.dir_in];

    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t vc = 0;
        int32_t next_id = TopologyGraph[route_data.current_id][r];

        if (prev_id > next_id) vc = 1;
        else vc = 0;

        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelNextIDGraterCurrentDirectionInverse::TEST_RoutingChannelNextIDGraterCurrentDirectionInverse(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelNextIDGraterCurrentDirectionInverse::Route(Router& router, const RouteData& route_data)
{
    std::vector<int32_t> result;

    int32_t dist = route_data.src_id - route_data.dst_id;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t vc = 0;
        int32_t next_id = TopologyGraph[route_data.current_id][r];

        if (dist > 0 && next_id > route_data.current_id) vc = 0;
        if (dist > 0 && next_id < route_data.current_id) vc = 1;
        if (dist < 0 && next_id > route_data.current_id) vc = 1;
        if (dist < 0 && next_id < route_data.current_id) vc = 0;

        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse::TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse::Route(Router& router, const RouteData& route_data)
{
    std::vector<int32_t> result;

    int32_t prev_id = route_data.current_id;
    if (route_data.dir_in < TopologyGraph[route_data.current_id].size())
        prev_id = TopologyGraph[route_data.current_id][route_data.dir_in];

    int32_t dist = route_data.src_id - route_data.dst_id;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t vc = 0;// prev_vc;
        int32_t next_id = TopologyGraph[route_data.current_id][r];

        if (dist > 0 && prev_id > route_data.current_id) vc = 0;
        if (dist > 0 && prev_id < route_data.current_id) vc = 1;
        if (dist < 0 && prev_id > route_data.current_id) vc = 1;
        if (dist < 0 && prev_id < route_data.current_id) vc = 0;

        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelSourceID::TEST_RoutingChannelSourceID(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelSourceID::Route(Router& router, const RouteData& route_data)
{
    std::vector<int32_t> result;
    int32_t vc = route_data.src_id < TopologyGraph.size() / 2 ? 0 : 1;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];

        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelDestinationID::TEST_RoutingChannelDestinationID(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelDestinationID::Route(Router& router, const RouteData& route_data)
{
    std::vector<int32_t> result;
    int32_t vc = route_data.dst_id < TopologyGraph.size() / 2 ? 0 : 1;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];

        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelInputLinkLength::TEST_RoutingChannelInputLinkLength(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelInputLinkLength::Route(Router& router, const RouteData& route_data)
{
    int32_t max_length = std::numeric_limits<int32_t>::min();
    for (int32_t r = 0; r < TopologyGraph[route_data.current_id].size(); r++)
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];
        if (abs(next_id - route_data.current_id) > max_length)
            max_length = abs(next_id - route_data.current_id);
    }

    int32_t prev_id = route_data.current_id;
    if (route_data.dir_in < TopologyGraph[route_data.current_id].size())
        prev_id = TopologyGraph[route_data.current_id][route_data.dir_in];
    int32_t vc = 0;
    if (abs(prev_id - route_data.current_id) == max_length) vc = 1;

    std::vector<int32_t> result;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];

        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
    if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsGrater0::TEST_RoutingChannelHopsGrater0(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelHopsGrater0::Route(Router& router, const RouteData& route_data)
{
    int32_t vc = route_data.hop_no <= 0 ? 0 : 1;
    std::vector<int32_t> result;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];
        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsGrater1::TEST_RoutingChannelHopsGrater1(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelHopsGrater1::Route(Router& router, const RouteData& route_data)
{
    int32_t vc = route_data.hop_no <= 1 ? 0 : 1;
    std::vector<int32_t> result;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];
        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsGrater2::TEST_RoutingChannelHopsGrater2(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelHopsGrater2::Route(Router& router, const RouteData& route_data)
{
    int32_t vc = route_data.hop_no <= 2 ? 0 : 1;
    std::vector<int32_t> result;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];
        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsSwitchNotMod2::TEST_RoutingChannelHopsSwitchNotMod2(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelHopsSwitchNotMod2::Route(Router& router, const RouteData& route_data)
{
    int32_t prev_id = route_data.current_id;
    if (route_data.dir_in < TopologyGraph[route_data.current_id].size())
        prev_id = TopologyGraph[route_data.current_id][route_data.dir_in];
    int32_t prev_vc = 0;
    for (int32_t i = 0; i < TopologyGraph[route_data.current_id].links_to(prev_id).size(); i++)
        if (TopologyGraph[route_data.current_id].links_to(prev_id)[i] == route_data.dir_in)
            prev_vc = i;

    int32_t vc = route_data.hop_no % 2 ? prev_vc : !prev_vc;
    std::vector<int32_t> result;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];
        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsSwitchNotMod3::TEST_RoutingChannelHopsSwitchNotMod3(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelHopsSwitchNotMod3::Route(Router& router, const RouteData& route_data)
{
    int32_t prev_id = route_data.current_id;
    if (route_data.dir_in < TopologyGraph[route_data.current_id].size())
        prev_id = TopologyGraph[route_data.current_id][route_data.dir_in];
    int32_t prev_vc = 0;
    for (int32_t i = 0; i < TopologyGraph[route_data.current_id].links_to(prev_id).size(); i++)
        if (TopologyGraph[route_data.current_id].links_to(prev_id)[i] == route_data.dir_in)
            prev_vc = i;

    int32_t vc = route_data.hop_no % 3 ? prev_vc : !prev_vc;
    std::vector<int32_t> result;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];
        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsSwitchNotMod4::TEST_RoutingChannelHopsSwitchNotMod4(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<int32_t> TEST_RoutingChannelHopsSwitchNotMod4::Route(Router& router, const RouteData& route_data)
{
    int32_t prev_id = route_data.current_id;
    if (route_data.dir_in < TopologyGraph[route_data.current_id].size())
        prev_id = TopologyGraph[route_data.current_id][route_data.dir_in];
    int32_t prev_vc = 0;
    for (int32_t i = 0; i < TopologyGraph[route_data.current_id].links_to(prev_id).size(); i++)
        if (TopologyGraph[route_data.current_id].links_to(prev_id)[i] == route_data.dir_in)
            prev_vc = i;

    int32_t vc = route_data.hop_no % 4 ? prev_vc : !prev_vc;
    std::vector<int32_t> result;
    for (int32_t r : Table[route_data.current_id][route_data.dst_id])
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];
        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}
