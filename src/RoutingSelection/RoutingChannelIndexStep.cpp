#include "RoutingChannelIndexStep.h"



RoutingChannelIndexStep::RoutingChannelIndexStep(const Graph& graph) :
    TopologyGraph(graph)
{
}

std::vector<int32_t> RoutingChannelIndexStep::Route(Router& router, const RouteData& route_data)
{
    const auto& routes = router.routing_table[route_data.dst_id];
    std::vector<int32_t> result;

    int32_t prev_id = route_data.current_id; 
    if (route_data.dir_in < TopologyGraph[route_data.current_id].size())
        prev_id = TopologyGraph[route_data.current_id][route_data.dir_in];
    int32_t prev_vc = 0;
    for (int32_t i = 0; i < TopologyGraph[route_data.current_id].links_to(prev_id).size(); i++)
    {
        if (TopologyGraph[route_data.current_id].links_to(prev_id)[i] == route_data.dir_in)
        {
            prev_vc = i;
        }
    }

    for (int32_t r : routes)
    {
        int32_t next_id = TopologyGraph[route_data.current_id][r];
        if (next_id == 0 || next_id == TopologyGraph.size() - 1) prev_vc = 1;
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[prev_vc]) result.push_back(r);
        //if (route_data.hop_no % 2)
        //{
        //    if (next_id > route_data.current_id)
        //    {
        //        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[0]) result.push_back(r);
        //    }
        //    if (next_id < route_data.current_id)
        //    {
        //        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[1]) result.push_back(r);
        //    }
        //}
        //else 
        //{
        //    if (next_id > route_data.current_id)
        //    {
        //        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[2]) result.push_back(r);
        //    }
        //    if (next_id < route_data.current_id)
        //    {
        //        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[3]) result.push_back(r);
        //    }
        //}
        //if (next_id > route_data.current_id)
        //{
        //    if (r == TopologyGraph[route_data.current_id].links_to(next_id)[0]) result.push_back(r);
        //}
        //if (next_id < route_data.current_id)
        //{
        //    if (r == TopologyGraph[route_data.current_id].links_to(next_id)[1]) result.push_back(r);
        //}
    }

    if (result.size() < 1)
    {
        throw std::runtime_error("SUPERSHIT");
    }
    return std::move(result);
}
