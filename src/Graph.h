#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <map>



class Graph
{
public:
	static constexpr int32_t EmptyLink = -1;

	class Node
	{
	private:
		std::vector<int32_t> Links;

	public:
		void push_back(int32_t id) { Links.push_back(id); }
		int32_t size() const { return Links.size(); }
		int32_t operator[](int32_t i) const { return Links[i]; }
		int32_t& operator[](int32_t i) { return Links[i]; }
		int32_t link_to(int32_t id) const 
		{
			for (int32_t i = 0; i < Links.size(); i++)
				if (Links[i] == id) return i;
			return -1;
		}
	};

private:
	std::vector<Node> Nodes;

public:
	Graph(const std::string& path);
	Graph();

	void resize(int32_t size);
	void push_back(Node&& node);

	int32_t size() const;

	Node& operator[](int32_t i);
	const Node& operator[](int32_t i) const;

	friend std::ostream& operator<<(std::ostream& os, const Graph& g);
};

