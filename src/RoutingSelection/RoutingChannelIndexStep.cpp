#include "RoutingChannelIndexStep.h"



RoutingChannelIndexStep::RoutingChannelIndexStep(const Graph& graph) :
    TopologyGraph(graph)
{
}

std::vector<int32_t> RoutingChannelIndexStep::Route(Router& router, const RouteData& route_data)
{
    std::vector<int32_t> result;

    int32_t prev_id = route_data.current_id; 
    if (route_data.dir_in < TopologyGraph[route_data.current_id].size())
        prev_id = TopologyGraph[route_data.current_id][route_data.dir_in];
    //int32_t prev_vc = 0;
    //for (int32_t i = 0; i < TopologyGraph[route_data.current_id].links_to(prev_id).size(); i++)
    //    if (TopologyGraph[route_data.current_id].links_to(prev_id)[i] == route_data.dir_in)
    //        prev_vc = i;

    int32_t dist = route_data.src_id - route_data.dst_id;
    //if (route_data.current_id % 14 == 0 && route_data.current_id != route_data.src_id) vc = 1;
    for (int32_t r : router.routing_table[route_data.dst_id])
    {
        int32_t vc = 0;// prev_vc;
        int32_t next_id = TopologyGraph[route_data.current_id][r];

        //if (dist > 0 && next_id > route_data.current_id) vc = 0;
        //if (dist > 0 && next_id < route_data.current_id) vc = 1;
        //if (dist < 0 && next_id > route_data.current_id) vc = 1;
        //if (dist < 0 && next_id < route_data.current_id) vc = 0;

        //if (next_id > route_data.current_id) vc = 0;
        //if (next_id < route_data.current_id) vc = 1;

        if (prev_id > route_data.current_id) vc = 0;
        if (prev_id < route_data.current_id) vc = 1;

        if (vc >= TopologyGraph[route_data.current_id].links_to(next_id).size())
            throw std::runtime_error("Routing error: Invalid virtual channel [" + std::to_string(vc) + "].");
        if (r == TopologyGraph[route_data.current_id].links_to(next_id)[vc]) result.push_back(r);
    }

    if (result.size() < 1) throw std::runtime_error("Routing error: No paths found.");
    return std::move(result);
}
