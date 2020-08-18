#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <map>



class Graph
{
public:
	static constexpr std::int32_t EmptyLink = -1;

	class Node
	{
	private:
		std::vector<std::int32_t> Links;

	public:
		void push_back(std::int32_t id, std::int32_t ch = 1) { while (ch--) Links.push_back(id); }
		std::int32_t size() const { return Links.size(); }
		std::int32_t operator[](std::int32_t i) const { return Links[i]; }
		std::int32_t& operator[](std::int32_t i) { return Links[i]; }
		std::vector<std::int32_t> links_to(std::int32_t id) const 
		{
			std::vector<std::int32_t> result;
			result.reserve(2);
			for (std::int32_t i = 0; i < Links.size(); i++)
				if (Links[i] == id) result.push_back(i);
			return std::move(result);
		}
	};
	struct PathNode
	{
		std::int32_t IRelay;
		std::int32_t NodeID;
		std::int32_t ORelay;
	};

private:
	std::vector<Node> Nodes;

	void find_shortest(std::vector<std::vector<PathNode>>& paths, std::vector<PathNode> path, const std::vector<std::int32_t>& weights, std::int32_t dest) const;
	void find_shortest(std::vector<std::vector<std::int32_t>>& paths, std::vector<std::int32_t> path, const std::vector<std::int32_t>& weights, std::int32_t dest) const;

public:
	Graph(const std::string& path);
	Graph();

	void resize(std::int32_t size);
	void push_back(Node&& node);

	std::int32_t size() const;

	Node& operator[](std::int32_t i);
	const Node& operator[](std::int32_t i) const;

	friend std::ostream& operator<<(std::ostream& os, const Graph& g);

	std::vector<std::vector<PathNode>> get_paths(std::int32_t from, std::int32_t to) const;
	std::vector<std::vector<std::int32_t>> get_simple_paths(std::int32_t from, std::int32_t to) const;
};

