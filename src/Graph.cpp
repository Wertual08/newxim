#include "Graph.h"
#include <fstream>
#include <iostream>
#include <sstream>



Graph::Graph(const std::string& path)
{
	std::ifstream fin(path);
	std::string line;
	int32_t l = 0;
	while (std::getline(fin, line))
	{
		l++;
		std::istringstream sin(line);
		Node node;
		int32_t val;
		while (!sin.eof() && sin >> val) node.Links.push_back(val);
		if (sin.fail()) throw std::runtime_error("Error reading topology[line=" + std::to_string(l) + "]: Bad graph format." );
		Nodes.push_back(std::move(node));
	}
	fin.close();

	//for (auto& it : Nodes)
	//{
	//	for (auto& s : it.Links) std::cout << s << " ";
	//	std::cout << '\n';
	//}
}

int32_t Graph::size() const
{
	return Nodes.size();
}

Graph::Node& Graph::operator[](int32_t i)
{
	return Nodes[i];
}
const Graph::Node& Graph::operator[](int32_t i) const
{
	return Nodes[i];
}
