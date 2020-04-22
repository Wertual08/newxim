#pragma once
#include <string>
#include <vector>
#include <set>



class RoutingTable
{
public:
	// ID to Relay relations
	using Node = std::vector<std::vector<int32_t>>;

private:
	// ID to Relations
	std::vector<Node> Nodes;
	bool Valid;

public:
	RoutingTable();

	// Load routing table from file. Returns true if ok, false otherwise
	bool Load(const std::string& path);

	Node& operator[](int32_t node_id);
	const Node& operator[](int32_t node_id) const;

	bool IsValid() const;
};
