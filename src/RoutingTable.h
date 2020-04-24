#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <set>
#include "Graph.h"



class RoutingTable
{
public:
	// ID to Relay relations
	using Node = std::vector<std::vector<int32_t>>;

private:
	// ID to Relations
	std::vector<Node> Nodes;
	bool Valid;

	bool LoadDijkstra(const Graph& graph);
	bool LoadDijkstraMultipath(const Graph& graph);

public:
	RoutingTable();
	RoutingTable(const std::string& path);
	RoutingTable(const Graph& graph, const std::string& generator = "DEFAULT");

	// Load routing table from file. Returns true if ok, false otherwise
	bool Load(const std::string& path);
	bool Load(const Graph& graph, const std::string& generator = "DEFAULT");
	void MakeValid();

	void push_back(Node&& node);

	Node& operator[](int32_t node_id);
	const Node& operator[](int32_t node_id) const;

	bool IsValid() const;

	friend std::ostream& operator<<(std::ostream& os, const RoutingTable& rt);
};
