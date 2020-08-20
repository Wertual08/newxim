#include "Graph.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>



void Graph::find_shortest(std::vector<std::vector<PathNode>>& paths, std::vector<PathNode> path, const std::vector<std::int32_t>& weights, std::int32_t dest) const
{
	PathNode node = path[path.size() - 1];

	PathNode new_node;
	new_node.NodeID = at(node.NodeID)[node.ORelay];

	auto links_to = at(node.NodeID).links_to(new_node.NodeID);
	std::int32_t i = 0;
	while (links_to[i] != node.ORelay) i++;
	new_node.IRelay = at(new_node.NodeID).links_to(node.NodeID)[i];

	if (new_node.NodeID == dest)
	{
		new_node.ORelay = -1;
		path.push_back(new_node);
		paths.push_back(path);
		return;
	}

	std::int32_t required_weight = weights[new_node.NodeID] - 1;
	for (std::int32_t i = 0; i < at(new_node.NodeID).size(); i++)
	{
		if (weights[at(new_node.NodeID)[i]] == required_weight)
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
	for (std::int32_t i = 0; i < at(node).size(); i++)
	{
		if (weights[at(node)[i]] == required_weight)
		{
			auto new_path = path;
			new_path.push_back(at(node)[i]);
			if (at(node)[i] == dest) paths.push_back(new_path);
			else find_shortest(paths, new_path, weights, dest);
		}
	}
}

std::istream& operator>>(std::istream& is, Graph& g)
{
	std::string line;
	std::int32_t l = 0;
	while (std::getline(is, line))
	{
		if (line.empty() || line[0] == '#') continue;
		if (line == "[GRAPH_END]") break;

		l++;
		std::istringstream sin(line);
		GraphNode node;
		std::int32_t val;
		while (!sin.eof() && sin >> val) node.push_back(val);
		if (sin.fail()) throw std::runtime_error("Error reading topology [line=" + std::to_string(l) + "]: Bad graph format.");
		g.push_back(std::move(node));
	}

	return is;
}
std::ostream& operator<<(std::ostream& os, const Graph& g)
{
	os << "[\n";
	for (auto& n : g)
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
	std::vector<std::int32_t> weights(size(), inf);
	std::vector<bool> visited(size(), false);
	weights[to] = 0;

	std::int32_t min_index, min;
	do
	{
		min_index = min = inf;

		for (std::int32_t j = 0; j < size(); j++)
		{
			if (!visited[j] && weights[j] < min)
			{
				min = weights[j];
				min_index = j;
			}
		}

		if (min_index != inf)
		{
			for (std::int32_t j = 0; j < at(min_index).size(); j++)
			{
				std::int32_t id = at(min_index)[j];
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
	for (std::int32_t i = 0; i < at(from).size(); i++)
	{
		if (weights[at(from)[i]] == required_weight)
		{
			find_shortest(paths, std::vector<PathNode>(1, { -1, from, i }), weights, to);
		}
	}

	return paths;
}
std::vector<std::vector<std::int32_t>> Graph::get_simple_paths(std::int32_t from, std::int32_t to) const
{
	constexpr std::int32_t inf = std::numeric_limits<std::int32_t>::max();
	std::vector<std::int32_t> weights(size(), inf);
	std::vector<bool> visited(size(), false);
	weights[to] = 0;

	std::int32_t min_index, min;
	do
	{
		min_index = min = inf;

		for (std::int32_t j = 0; j < size(); j++)
		{
			if (!visited[j] && weights[j] < min)
			{
				min = weights[j];
				min_index = j;
			}
		}

		if (min_index != inf)
		{
			for (std::int32_t j = 0; j < at(min_index).size(); j++)
			{
				std::int32_t id = at(min_index)[j];
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

Graph Graph::directed_subtree(std::int32_t root_node) const
{
	Graph result;
	result.resize(size());
	std::vector<bool> visited(size(), false);
	visited[root_node] = true;

	std::vector<std::int32_t> nodes_to_visit(1, root_node);
	for (std::size_t i = 0; i < nodes_to_visit.size(); i++)
	{
		std::int32_t node = nodes_to_visit[i];
		
		for (std::int32_t sub_node : at(node))
		{
			if (!visited[sub_node])
			{
				visited[sub_node] = true;
				result[node].push_back(sub_node);
				nodes_to_visit.push_back(sub_node);
			}
		}
	}

	return result;
}
Graph Graph::subtree(std::int32_t root_node) const
{
	Graph result;
	result.resize(size());
	std::vector<bool> visited(size(), false);
	visited[root_node] = true;

	std::vector<std::int32_t> nodes_to_visit(1, root_node);
	for (std::size_t i = 0; i < nodes_to_visit.size(); i++)
	{
		std::int32_t node = nodes_to_visit[i];

		for (std::int32_t sub_node : at(node))
		{
			if (!visited[sub_node])
			{
				visited[sub_node] = true;
				result[node].push_back(sub_node);
				result[sub_node].push_back(node);
				nodes_to_visit.push_back(sub_node);
			}
		}
	}

	return result;
}
