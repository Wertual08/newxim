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

	bool LoadDijkstraDeadlockFree(const Graph& graph);
	bool LoadDijkstra(const Graph& graph);
	bool LoadDijkstraMultipath(const Graph& graph);
	bool LoadUpDown(const Graph& graph);
	bool LoadMeshXY(const Graph& graph);
	bool LoadCirculantPairExchange(const Graph& graph);
	bool LoadCirculantClockwise(const Graph& graph);
	bool LoadCirculantAdaptive(const Graph& graph);
	bool LoadCirculantMultiplicative(const Graph& graph);
	bool LoadDeltaDistanceVector(const Graph &graph);

public:
	RoutingTable();
	RoutingTable(const std::string& path);
	RoutingTable(const Graph& graph, const std::string& generator = "DEFAULT");
	RoutingTable(const Graph& graph, const Graph& sub_graph, const std::string& generator = "DEFAULT");

	// Load routing table from file. Returns true if ok, false otherwise
	bool Load(const std::string& path);
	bool Load(const Graph& graph, const std::string& generator = "DEFAULT");

	void push_back(Node&& node);

	Node& operator[](std::int32_t node_id);
	const Node& operator[](std::int32_t node_id) const;

	bool IsValid() const;

	friend std::ostream& operator<<(std::ostream& os, const RoutingTable& rt);
};
