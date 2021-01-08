#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <set>
#include "Graph.hpp"



class RoutingTable
{
public:
	// ID to Relay relations
	using Node = std::vector<std::vector<std::int32_t>>;

private:
	// ID to Relations
	std::vector<Node> Nodes;

	bool LoadDijkstra(const Graph& graph);
	bool LoadUpDown(const Graph& graph);
	bool LoadMeshXY(const Graph& graph);
	bool LoadCirculantPairExchange(const Graph& graph);
	bool LoadCirculantClockwise(const Graph& graph);
	bool LoadCirculantAdaptive(const Graph& graph);
	bool LoadCirculantMultiplicative(const Graph& graph);
	bool LoadGreedyPromotion(const Graph &graph);

public:
	RoutingTable();
	RoutingTable(const std::string& path);
	RoutingTable(const Graph& graph, const std::string& generator = "DEFAULT");
	RoutingTable(const Graph& graph, const Graph& sub_graph, const std::string& generator = "DEFAULT");

	bool Load(const std::string& path);
	bool Load(const Graph& graph, const std::string& generator = "DEFAULT");

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
