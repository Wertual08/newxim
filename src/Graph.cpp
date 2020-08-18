#include "Graph.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>



void Graph::find_shortest(std::vector<std::vector<PathNode>>& paths, std::vector<PathNode> path, const std::vector<std::int32_t>& weights, std::int32_t dest) const
{
	PathNode node = path[path.size() - 1];

	PathNode new_node;
	new_node.NodeID = Nodes[node.NodeID][node.ORelay];

	auto links_to = Nodes[node.NodeID].links_to(new_node.NodeID);
	std::int32_t i = 0;
	while (links_to[i] != node.ORelay) i++;
	new_node.IRelay = Nodes[new_node.NodeID].links_to(node.NodeID)[i];

	if (new_node.NodeID == dest)
	{
		new_node.ORelay = -1;
		path.push_back(new_node);
		paths.push_back(path);
		return;
	}

	std::int32_t required_weight = weights[new_node.NodeID] - 1;
	for (std::int32_t i = 0; i < Nodes[new_node.NodeID].size(); i++)
	{
		if (weights[Nodes[new_node.NodeID][i]] == required_weight)
		{
			auto new_path = path;
			new_node.ORelay = i;
			new_path.push_back(new_node);
			find_shortest(paths, new_path, weights, dest);
		}
	}
}
void Graph::find_shortest(std::vector<std::vector<std::int32_t>>& paths, std::vector<std::int32_t> path, const std::vector<std::int32_t>& weights, std::int32_t dest) const
{
	std::int32_t node = path[path.size() - 1];

	std::int32_t required_weight = weights[node] - 1;
	for (std::int32_t i = 0; i < Nodes[node].size(); i++)
	{
		if (weights[Nodes[node][i]] == required_weight)
		{
			auto new_path = path;
			new_path.push_back(Nodes[node][i]);
			if (Nodes[node][i] == dest) paths.push_back(new_path);
			else find_shortest(paths, new_path, weights, dest);
		}
	}
}

Graph::Graph(const std::string& path)
{
	std::ifstream fin(path);
	std::string line;
	std::int32_t l = 0;
	while (std::getline(fin, line))
	{
		if (line.empty() || line[0] == '#') continue;
		l++;
		std::istringstream sin(line);
		Node node;
		std::int32_t val;
		while (!sin.eof() && sin >> val) node.push_back(val);
		if (sin.fail()) throw std::runtime_error("Error reading topology[line=" + std::to_string(l) + "]: Bad graph format." );
		Nodes.push_back(std::move(node));
	}
	fin.close();
}

Graph::Graph()
{
}

void Graph::resize(std::int32_t size)
{
	Nodes.resize(size);
}
void Graph::push_back(Node&& node)
{
	Nodes.push_back(std::move(node));
}

std::int32_t Graph::size() const
{
	return Nodes.size();
}

Graph::Node& Graph::operator[](std::int32_t i)
{
	return Nodes[i];
}
const Graph::Node& Graph::operator[](std::int32_t i) const
{
	return Nodes[i];
}

std::ostream& operator<<(std::ostream& os, const Graph& g)
{
	os << "[\n";
	for (auto& n : g.Nodes)
	{
		os << '[';
		for (std::int32_t i = 0; i < n.size() - 1; i++) os << n[i] << ", ";
		if (n.size()) os << n[n.size() - 1];
		os << "],\n";
	}
	os << "]";
	return os;
}

std::vector<std::vector<Graph::PathNode>> Graph::get_paths(std::int32_t from, std::int32_t to) const
{
	constexpr std::int32_t inf = std::numeric_limits<std::int32_t>::max();
	std::vector<std::int32_t> weights(Nodes.size(), inf);
	std::vector<bool> visited(Nodes.size(), false);
	weights[to] = 0;

	std::int32_t min_index, min;
	do
	{
		min_index = min = inf;

		for (std::int32_t j = 0; j < Nodes.size(); j++)
		{
			if (!visited[j] && weights[j] < min)
			{
				min = weights[j];
				min_index = j;
			}
		}

		if (min_index != inf)
		{
			for (std::int32_t j = 0; j < Nodes[min_index].size(); j++)
			{
				std::int32_t id = Nodes[min_index][j];
				if (id >= 0)
				{
					std::int32_t t = min + 1; // 1 distance may be here...
					if (t < weights[id]) weights[id] = t;
				}
			}
			visited[min_index] = true;
		}
	} while (min_index != inf);

	std::vector<std::vector<PathNode>> paths;
	std::int32_t required_weight = weights[from] - 1;
	for (std::int32_t i = 0; i < Nodes[from].size(); i++)
	{
		if (weights[Nodes[from][i]] == required_weight)
		{
			find_shortest(paths, std::vector<PathNode>(1, { -1, from, i }), weights, to);
		}
	}

	return paths;
}
std::vector<std::vector<std::int32_t>> Graph::get_simple_paths(std::int32_t from, std::int32_t to) const
{
	constexpr std::int32_t inf = std::numeric_limits<std::int32_t>::max();
	std::vector<std::int32_t> weights(Nodes.size(), inf);
	std::vector<bool> visited(Nodes.size(), false);
	weights[to] = 0;

	std::int32_t min_index, min;
	do
	{
		min_index = min = inf;

		for (std::int32_t j = 0; j < Nodes.size(); j++)
		{
			if (!visited[j] && weights[j] < min)
			{
				min = weights[j];
				min_index = j;
			}
		}

		if (min_index != inf)
		{
			for (std::int32_t j = 0; j < Nodes[min_index].size(); j++)
			{
				std::int32_t id = Nodes[min_index][j];
				if (id >= 0)
				{
					std::int32_t t = min + 1; // 1 distance may be here...
					if (t < weights[id]) weights[id] = t;
				}
			}
			visited[min_index] = true;
		}
	} while (min_index != inf);

	std::vector<std::vector<std::int32_t>> paths;
	find_shortest(paths, std::vector<std::int32_t>(1, from), weights, to);
	return paths;
}
