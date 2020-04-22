#pragma once
#include <string>
#include <vector>
#include <map>



class Graph
{
public:
	static constexpr int32_t EmptyLink = -1;

	class Node
	{
		friend Graph;
	private:
		std::vector<int32_t> Links;

	public:
		int32_t size() const { return Links.size(); }
		int32_t operator[](int32_t i) const { return Links[i]; }
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

	int32_t size() const;

	Node& operator[](int32_t i);
	const Node& operator[](int32_t i) const;
};

