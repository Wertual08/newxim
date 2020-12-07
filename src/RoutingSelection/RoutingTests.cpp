#include "RoutingTests.hpp"
#include "Hardware/Routers/Router.hpp"



std::unique_ptr<RoutingAlgorithm> FindTestRouting(const std::string& name, const Configuration& config, const RoutingTable& table)
{
    if (name == "TEST_RoutingChannelNextIDGraterCurrent") return std::make_unique<TEST_RoutingChannelNextIDGraterCurrent>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelPrevIDGraterCurrent") return std::make_unique<TEST_RoutingChannelPrevIDGraterCurrent>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelPrevIDGraterNext") return std::make_unique<TEST_RoutingChannelPrevIDGraterNext>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelNextIDGraterCurrentDirectionInverse") return std::make_unique<TEST_RoutingChannelNextIDGraterCurrentDirectionInverse>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse") return std::make_unique<TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelSourceID") return std::make_unique<TEST_RoutingChannelSourceID>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelDestinationID") return std::make_unique<TEST_RoutingChannelDestinationID>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelInputLinkLength") return std::make_unique<TEST_RoutingChannelInputLinkLength>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelHopsGrater0") return std::make_unique<TEST_RoutingChannelHopsGrater0>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelHopsGrater1") return std::make_unique<TEST_RoutingChannelHopsGrater1>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelHopsGrater2") return std::make_unique<TEST_RoutingChannelHopsGrater2>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelHopsSwitchNotMod2") return std::make_unique<TEST_RoutingChannelHopsSwitchNotMod2>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelHopsSwitchNotMod3") return std::make_unique<TEST_RoutingChannelHopsSwitchNotMod3>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelHopsSwitchNotMod4") return std::make_unique<TEST_RoutingChannelHopsSwitchNotMod4>(config.TopologyGraph(), table);
    if (name == "TEST_RoutingChannelSwitchNode0AndNDiv2") return std::make_unique<TEST_RoutingChannelSwitchNode0AndNDiv2>(config.TopologyGraph(), table);
	return nullptr;
}


TEST_RoutingChannelNextIDGraterCurrent::TEST_RoutingChannelNextIDGraterCurrent(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelNextIDGraterCurrent::Route(const Router& router, const Flit& flit) const
{
    std::vector<Connection> result;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t vc = 0;// prev_vc;
        std::int32_t next_id = TopologyGraph[router.LocalID][r];

        if (next_id > router.LocalID) vc = 1;
        else vc = 0;

        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelPrevIDGraterCurrent::TEST_RoutingChannelPrevIDGraterCurrent(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelPrevIDGraterCurrent::Route(const Router& router, const Flit& flit) const
{
    std::vector<Connection> result;

    std::int32_t prev_id = router.LocalID;
    if (flit.dir_in < TopologyGraph[router.LocalID].size())
        prev_id = TopologyGraph[router.LocalID][flit.dir_in];
   
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t vc = 0;
        std::int32_t next_id = TopologyGraph[router.LocalID][r];

        if (prev_id > router.LocalID) vc = 1;
        else vc = 0;

        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelPrevIDGraterNext::TEST_RoutingChannelPrevIDGraterNext(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelPrevIDGraterNext::Route(const Router& router, const Flit& flit) const
{
    std::vector<Connection> result;

    std::int32_t prev_id = router.LocalID;
    if (flit.dir_in < TopologyGraph[router.LocalID].size())
        prev_id = TopologyGraph[router.LocalID][flit.dir_in];

    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t vc = 0;
        std::int32_t next_id = TopologyGraph[router.LocalID][r];

        if (prev_id > next_id) vc = 1;
        else vc = 0;

        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelNextIDGraterCurrentDirectionInverse::TEST_RoutingChannelNextIDGraterCurrentDirectionInverse(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelNextIDGraterCurrentDirectionInverse::Route(const Router& router, const Flit& flit) const
{
    std::vector<Connection> result;

    std::int32_t dist = router.LocalID - flit.dst_id;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t vc = 0;
        std::int32_t next_id = TopologyGraph[router.LocalID][r];

        if (dist > 0 && next_id > router.LocalID) vc = 0;
        if (dist > 0 && next_id < router.LocalID) vc = 1;
        if (dist < 0 && next_id > router.LocalID) vc = 1;
        if (dist < 0 && next_id < router.LocalID) vc = 0;

        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse::TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelPrevIDGraterCurrentDirectionInverse::Route(const Router& router, const Flit& flit) const
{
    std::vector<Connection> result;

    std::int32_t prev_id = router.LocalID;
    if (flit.dir_in < TopologyGraph[router.LocalID].size())
        prev_id = TopologyGraph[router.LocalID][flit.dir_in];

    std::int32_t dist = router.LocalID - flit.dst_id;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t vc = 0;// prev_vc;
        std::int32_t next_id = TopologyGraph[router.LocalID][r];

        if (dist > 0 && prev_id > router.LocalID) vc = 0;
        if (dist > 0 && prev_id < router.LocalID) vc = 1;
        if (dist < 0 && prev_id > router.LocalID) vc = 1;
        if (dist < 0 && prev_id < router.LocalID) vc = 0;

        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelSourceID::TEST_RoutingChannelSourceID(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelSourceID::Route(const Router& router, const Flit& flit) const
{
    std::vector<Connection> result;
    std::int32_t vc = router.LocalID < TopologyGraph.size() / 2 ? 0 : 1;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];

        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelDestinationID::TEST_RoutingChannelDestinationID(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelDestinationID::Route(const Router& router, const Flit& flit) const
{
    std::vector<Connection> result;
    std::int32_t vc = flit.dst_id < TopologyGraph.size() / 2 ? 0 : 1;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];

        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelInputLinkLength::TEST_RoutingChannelInputLinkLength(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelInputLinkLength::Route(const Router& router, const Flit& flit) const
{
    std::int32_t max_length = std::numeric_limits<std::int32_t>::min();
    for (std::int32_t r = 0; r < TopologyGraph[router.LocalID].size(); r++)
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];
        if (abs(next_id - router.LocalID) > max_length)
            max_length = abs(next_id - router.LocalID);
    }

    std::int32_t prev_id = router.LocalID;
    if (flit.dir_in < TopologyGraph[router.LocalID].size())
        prev_id = TopologyGraph[router.LocalID][flit.dir_in];
    std::int32_t vc = 0;
    if (abs(prev_id - router.LocalID) == max_length) vc = 1;

    std::vector<Connection> result;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];

        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
    if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsGrater0::TEST_RoutingChannelHopsGrater0(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelHopsGrater0::Route(const Router& router, const Flit& flit) const
{
    std::int32_t vc = flit.hop_no <= 0 ? 0 : 1;
    std::vector<Connection> result;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];
        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsGrater1::TEST_RoutingChannelHopsGrater1(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelHopsGrater1::Route(const Router& router, const Flit& flit) const
{
    std::int32_t vc = flit.hop_no <= 1 ? 0 : 1;
    std::vector<Connection> result;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];
        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsGrater2::TEST_RoutingChannelHopsGrater2(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelHopsGrater2::Route(const Router& router, const Flit& flit) const
{
    std::int32_t vc = flit.hop_no <= 2 ? 0 : 1;
    std::vector<Connection> result;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];
        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsSwitchNotMod2::TEST_RoutingChannelHopsSwitchNotMod2(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelHopsSwitchNotMod2::Route(const Router& router, const Flit& flit) const
{
    std::int32_t prev_id = router.LocalID;
    if (flit.dir_in < TopologyGraph[router.LocalID].size())
        prev_id = TopologyGraph[router.LocalID][flit.dir_in];
    std::int32_t prev_vc = 0;
    for (std::int32_t i = 0; i < TopologyGraph[router.LocalID].links_to(prev_id).size(); i++)
        if (TopologyGraph[router.LocalID].links_to(prev_id)[i] == flit.dir_in)
            prev_vc = i;

    std::int32_t vc = flit.hop_no % 2 ? prev_vc : !prev_vc;
    std::vector<Connection> result;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];
        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsSwitchNotMod3::TEST_RoutingChannelHopsSwitchNotMod3(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelHopsSwitchNotMod3::Route(const Router& router, const Flit& flit) const
{
    std::int32_t prev_id = router.LocalID;
    if (flit.dir_in < TopologyGraph[router.LocalID].size())
        prev_id = TopologyGraph[router.LocalID][flit.dir_in];
    std::int32_t prev_vc = 0;
    for (std::int32_t i = 0; i < TopologyGraph[router.LocalID].links_to(prev_id).size(); i++)
        if (TopologyGraph[router.LocalID].links_to(prev_id)[i] == flit.dir_in)
            prev_vc = i;

    std::int32_t vc = flit.hop_no % 3 ? prev_vc : !prev_vc;
    std::vector<Connection> result;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];
        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}


TEST_RoutingChannelHopsSwitchNotMod4::TEST_RoutingChannelHopsSwitchNotMod4(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelHopsSwitchNotMod4::Route(const Router& router, const Flit& flit) const
{
    std::int32_t prev_id = router.LocalID;
    if (flit.dir_in < TopologyGraph[router.LocalID].size())
        prev_id = TopologyGraph[router.LocalID][flit.dir_in];
    std::int32_t prev_vc = 0;
    for (std::int32_t i = 0; i < TopologyGraph[router.LocalID].links_to(prev_id).size(); i++)
        if (TopologyGraph[router.LocalID].links_to(prev_id)[i] == flit.dir_in)
            prev_vc = i;

    std::int32_t vc = flit.hop_no % 4 ? prev_vc : !prev_vc;
    std::vector<Connection> result;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];
        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}

TEST_RoutingChannelSwitchNode0AndNDiv2::TEST_RoutingChannelSwitchNode0AndNDiv2(
    const Graph& graph, const RoutingTable& table) :
    TopologyGraph(graph), Table(table)
{
}
std::vector<Connection> TEST_RoutingChannelSwitchNode0AndNDiv2::Route(const Router& router, const Flit& flit) const
{
    std::int32_t prev_id = router.LocalID;
    if (flit.dir_in < TopologyGraph[router.LocalID].size())
        prev_id = TopologyGraph[router.LocalID][flit.dir_in];
    std::int32_t prev_vc = 0;
    for (std::int32_t i = 0; i < TopologyGraph[router.LocalID].links_to(prev_id).size(); i++)
        if (TopologyGraph[router.LocalID].links_to(prev_id)[i] == flit.dir_in)
            prev_vc = i;

    std::int32_t vc = router.LocalID == 0 || router.LocalID == TopologyGraph.size() / 2 ? 1 : prev_vc;
    std::vector<Connection> result;
    for (std::int32_t r : Table[router.LocalID][flit.dst_id])
    {
        std::int32_t next_id = TopologyGraph[router.LocalID][r];
        if (vc >= TopologyGraph[router.LocalID].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[router.LocalID].links_to(next_id)[vc]) result.push_back({ r, flit.vc_id });
    }
    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}