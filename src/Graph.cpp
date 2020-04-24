#include "Graph.h"
#include <fstream>
#include <iomanip>
#include <sstream>



Graph::Graph(const std::string& path)
{
	std::ifstream fin(path);
	std::string line;
	int32_t l = 0;
	while (std::getline(fin, line))
	{
		if (line.empty() || line[0] == '#') continue;
		l++;
		std::istringstream sin(line);
		Node node;
		int32_t val;
		while (!sin.eof() && sin >> val) node.push_back(val);
		if (sin.fail()) throw std::runtime_error("Error reading topology[line=" + std::to_string(l) + "]: Bad graph format." );
		Nodes.push_back(std::move(node));
	}
	fin.close();
}

Graph::Graph()
{
}

void Graph::resize(int32_t size)
{
	Nodes.resize(size);
}
void Graph::push_back(Node&& node)
{
	Nodes.push_back(std::move(node));
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

std::ostream& operator<<(std::ostream& os, const Graph& g)
{
	os << "[\n";
	for (auto& n : g.Nodes)
	{
		os << '[';
		for (int32_t i = 0; i < n.size() - 1; i++) os << n[i] << ", ";
		if (n.size()) os << n[n.size() - 1];
		os << "],\n";
	}
	os << "]";
	return os;
}