#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <set>
#include "Graph/Graph.hpp"



class RoutingTable
{
public:
	// ID to Relay relations
	using Node = std::vector<std::vector<std::int32_t>>;

private:
	// ID to Relations
	std::vector<Node> Nodes;

public:
	RoutingTable();
	RoutingTable(const std::string& path);

	bool Load(const std::string& path);

	void Init(const Graph& graph);
	bool LoadDijkstra(const Graph& graph);
	bool LoadUpDown(const Graph& graph);
	bool LoadMeshXY(const Graph& graph);
	bool LoadCirculantPairExchange(const Graph& graph);
	bool LoadCirculantClockwise(const Graph& graph);
	bool LoadCirculantAdaptive(const Graph& graph);
	bool LoadCirculantMultiplicative(const Graph& graph);
	bool LoadGreedyPromotion(const Graph& graph);

	void Adjust(const Graph &src_graph, const Graph &dst_graph);
	void Promote(const Graph &graph);

	void push_back(Node&& node);

	Node& operator[](std::int32_t node_id);
	const Node& operator[](std::int32_t node_id) const;

	bool IsValid() const;

private:
	void get_paths_helper(const Graph &graph, std::vector<std::vector<std::int32_t>> &paths, 
		std::vector<std::int32_t> path, std::int32_t next, std::int32_t d) const;
public:
	std::vector<std::vector<std::int32_t>> GetPaths(const Graph &graph, std::int32_t s, std::int32_t d) const;

	friend std::ostream& operator<<(std::ostream& os, const RoutingTable& rt);
};
