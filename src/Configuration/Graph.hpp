#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <map>



class AdjacencyMatrix
{
private:
	const size_t dimension;
	std::vector<std::int32_t> matrix;

public:
	friend std::ostream& operator<<(std::ostream& os, const AdjacencyMatrix& g);

	AdjacencyMatrix(std::size_t size) : dimension(size), matrix(size * size, 0) {}
	std::int32_t& at(int32_t x, int32_t y) { return matrix[y * dimension + x]; }
};

class GraphNode : public std::vector<std::int32_t>
{
public:
	void push_back(std::int32_t id, std::int32_t ch = 1) { while (ch--) std::vector<std::int32_t>::push_back(id); }
	std::vector<std::int32_t> links_to(std::int32_t id) const
	{
		std::vector<std::int32_t> result;
		result.reserve(2);
		for (std::int32_t i = 0; i < size(); i++)
			if (at(i) == id) result.push_back(i);
		return std::move(result);
	}
};
class Graph : public std::vector<GraphNode>
{
public:
	static constexpr std::int32_t EmptyLink = -1;

	struct PathNode
	{
		std::int32_t IRelay;
		std::int32_t NodeID;
		std::int32_t ORelay;
	};

private:
	void find_shortest(std::vector<std::vector<PathNode>>& paths, std::vector<PathNode> path, const std::vector<std::int32_t>& weights, std::int32_t src) const;
	void find_shortest(std::vector<std::vector<std::int32_t>>& paths, std::vector<std::int32_t> path, const std::vector<std::int32_t>& weights, std::int32_t src) const;

public:
	friend std::istream& operator>>(std::istream& is, Graph& g);
	friend std::ostream& operator<<(std::ostream& os, const Graph& g);

	Graph operator+(const Graph &g);

	std::vector<std::vector<PathNode>> get_paths(std::int32_t from, std::int32_t to) const;
	std::vector<std::vector<std::int32_t>> get_simple_paths(std::int32_t from, std::int32_t to) const;

	Graph directed_subtree(std::int32_t root_node = 0) const;
	Graph subgraph(const std::string& str);
	Graph tgen0_subtree(std::int32_t root_node = 0) const;
	Graph tgen1_subtree(std::int32_t root_node = 0) const;
	Graph tgen2_subtree(std::int32_t root_node = 0) const;
	Graph tgen3_subtree(std::int32_t root_node = 0) const;
	Graph tgen4_subtree(std::int32_t root_node = 0) const;
	Graph tgen5_subtree() const;

	std::int32_t wiener_index() const;

	AdjacencyMatrix adjacency_matrix() const;
};

