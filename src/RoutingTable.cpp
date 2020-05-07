#include "RoutingTable.h"
#include <fstream>



std::vector<std::string> split(const std::string& str, char s)
{
	std::vector<std::string> result;
	size_t first = -1, ind = 0;
	do
	{
		ind = str.find(s, first + 1);
		if (ind != std::string::npos && ind - first > 1) result.push_back(std::move(str.substr(first + 1, ind - first - 1)));
		else if (ind == std::string::npos && str.size() - first > 1) result.push_back(std::move(str.substr(first + 1)));
		first = ind;
	} while (first != std::string::npos);
	return std::move(result);
}

bool RoutingTable::LoadDijkstra(const Graph& graph)
{
	Nodes.resize(graph.size());
	for (int32_t i = 0; i < Nodes.size(); i++)
		Nodes[i].resize(graph.size());

	constexpr int32_t inf = std::numeric_limits<int32_t>::max();

	for (int32_t i = 0; i < graph.size(); i++)
	{
		std::vector<int32_t> weights(graph.size(), inf);
		std::vector<bool> visited(graph.size(), false);
		weights[i] = 0;

		int32_t min_index, min;
		do
		{
			min_index = min = inf;

			for (int32_t j = 0; j < graph.size(); j++)
			{ 
				if (!visited[j] && weights[j] < min)
				{ 
					min = weights[j];
					min_index = j;
				}
			}

			if (min_index != inf)
			{
				for (int32_t j = 0; j < graph[min_index].size(); j++)
				{
					int32_t id = graph[min_index][j];
					if (id >= 0)
					{
						int32_t t = min + 1; // 1 distance may be here...
						if (t < weights[id]) weights[id] = t;
					}
				}
				visited[min_index] = true;
			}
		} 
		while (min_index != inf);

		for (int32_t j = 0; j < graph.size(); j++)
		{
			Nodes[j][i].push_back(0);
			auto& current_relay = Nodes[j][i][0];

			for (int32_t k = 1; k < graph[j].size(); k++)
			{
				int32_t id = graph[j][k];
				if (id >= 0)
				{
					int32_t t = weights[j] - 1;
					if (t == weights[id]) current_relay = k;
				}
			}
		}
	}
	return true;
}
bool RoutingTable::LoadDijkstraMultipath(const Graph& graph)
{
	Nodes.resize(graph.size());
	for (int32_t i = 0; i < Nodes.size(); i++)
		Nodes[i].resize(graph.size());

	constexpr int32_t inf = std::numeric_limits<int32_t>::max();

	for (int32_t i = 0; i < graph.size(); i++)
	{
		std::vector<int32_t> weights(graph.size(), inf);
		std::vector<bool> visited(graph.size(), false);
		weights[i] = 0;

		int32_t min_index, min;
		do
		{
			min_index = min = inf;

			for (int32_t j = 0; j < graph.size(); j++)
			{
				if (!visited[j] && weights[j] < min)
				{
					min = weights[j];
					min_index = j;
				}
			}

			if (min_index != inf)
			{
				for (int32_t j = 0; j < graph[min_index].size(); j++)
				{
					int32_t id = graph[min_index][j];
					if (id >= 0)
					{
						int32_t t = min + 1; // 1 distance may be here...
						if (t < weights[id]) weights[id] = t;
					}
				}
				visited[min_index] = true;
			}
		} while (min_index != inf);

		for (int32_t j = 0; j < graph.size(); j++)
		{
			if (j == i) Nodes[j][i].push_back(graph[i].size());
			else
			{
				auto& current_relay = Nodes[j][i];

				for (int32_t k = 0; k < graph[j].size(); k++)
				{
					int32_t id = graph[j][k];
					if (id >= 0)
					{
						int32_t t = weights[j] - 1;
						if (t == weights[id]) current_relay.push_back(k);
					}
				}
			}
		}
	}
	return true;
}

RoutingTable::RoutingTable()
{
}
RoutingTable::RoutingTable(const std::string& path)
{
	Load(path);
}
RoutingTable::RoutingTable(const Graph& graph, const std::string& generator)
{
	Load(graph, generator);
}

bool RoutingTable::Load(const std::string& path)
{
	std::ifstream fin(path, std::ios::in);

	if (!fin) return false;

	Nodes.clear();

	std::string line;
	while (getline(fin, line))
	{
		if (line.size() > 0 && line[0] != '%')
		{
			std::vector<std::vector<int32_t>> node;
			auto routes = split(line, ';');
			for (std::string& route : routes)
			{
				auto relays = split(route, ',');
				std::vector<int32_t> res_relays;
				for (std::string& relay : relays)
					res_relays.push_back(std::stoi(relay));
				node.push_back(std::move(res_relays));
			}
			Nodes.push_back(std::move(node));
		}
	}

	return true;
}

bool RoutingTable::Load(const Graph& graph, const std::string& generator)
{
	if (generator == "DEFAULT") return LoadDijkstra(graph);
	if (generator == "DIJKSTRA") return LoadDijkstra(graph);
	if (generator == "DIJKSTRA_MULTIPATH") return LoadDijkstraMultipath(graph);
	return false;
}
bool RoutingTable::LoadMeshXY(const Graph& graph, int32_t w, int32_t h)
{
	Nodes.resize(graph.size());

	for (int32_t x = 0; x < w; x++)
	{
		for (int32_t y = 0; y < h; y++)
		{
			int32_t id = y * w + x;
			int32_t du = -1;
			int32_t dl = -1;
			int32_t dd = -1;
			int32_t dr = -1;
			if (y + 1 < h) du = graph[id].links_to((y + 1) * w + x)[0];
			if (x - 1 >= 0) dl = graph[id].links_to(y * w + x - 1)[0];
			if (y - 1 >= 0) dd = graph[id].links_to((y - 1) * w + x)[0];
			if (x + 1 < w) dr = graph[id].links_to(y * w + x + 1)[0];

			Nodes[id].resize(graph.size());
			for (int dy = 0; dy < h; dy++)
			{
				for (int dx = 0; dx < w; dx++)
				{
					int32_t did = dy * w + dx;

					if (dx > x) Nodes[id][did].push_back(dr); //return (int)Direction.East;
					else if (dx < x) Nodes[id][did].push_back(dl); //return (int)Direction.West;
					else if (dy > y) Nodes[id][did].push_back(du); //return (int)Direction.South;
					else if (dy < y) Nodes[id][did].push_back(dd); //return (int)Direction.North;
					else Nodes[id][did].push_back(graph[id].size());
				}
			}
		}
	}

	return true;
}
bool RoutingTable::LoadTorusXY(const Graph& graph, int32_t w, int32_t h)
{
	Nodes.resize(graph.size());

	for (int32_t x = 0; x < w; x++)
	{
		for (int32_t y = 0; y < h; y++)
		{
			int32_t id = y * w + x;
			int32_t du = -1;
			int32_t dl = -1;
			int32_t dd = -1;
			int32_t dr = -1;
			if (y + 1 < h) du = graph[id].links_to((y + 1) * w + x)[0];
			else du = graph[id].links_to(x)[0];
			if (x - 1 >= 0) dl = graph[id].links_to(y * w + x - 1)[0];
			else dl = graph[id].links_to(y * w + w - 1)[0];
			if (y - 1 >= 0) dd = graph[id].links_to((y - 1) * w + x)[0];
			else dd = graph[id].links_to((h - 1) * w + x)[0];
			if (x + 1 < w) dr = graph[id].links_to(y * w + x + 1)[0];
			else dr = graph[id].links_to(y * w)[0];

			Nodes[id].resize(graph.size());
			for (int dy = 0; dy < h; dy++)
			{
				for (int dx = 0; dx < w; dx++)
				{
					int32_t did = dy * w + dx;
					if (did == id)
					{
						Nodes[id][did].push_back(graph[id].size());
						continue;
					}

					int32_t udist = dy >= y ? dy - y : dy + h - y;
					int32_t ldist = x >= dx ? x - dx : x + w - dx;
					int32_t ddist = y >= dy ? y - dy : y + h - dy;
					int32_t rdist = dx >= x ? dx - x : dx + w - x;

					if (ldist >= rdist && rdist != 0) Nodes[id][did].push_back(dr); //return (int)Direction.East;
					else if (ldist < rdist) Nodes[id][did].push_back(dl); //return (int)Direction.West;
					else if (ddist >= udist && udist != 0) Nodes[id][did].push_back(du); //return (int)Direction.South;
					else Nodes[id][did].push_back(dd); //return (int)Direction.North;
					//else Nodes[id][did].push_back(graph[id].size());
				}
			}
		}
	}

	return true;
}
bool RoutingTable::LoadCirculant(const Graph& graph)
{
	Nodes.resize(graph.size());
	for (int32_t i = 0; i < Nodes.size(); i++)
		Nodes[i].resize(graph.size());

	constexpr int32_t inf = std::numeric_limits<int32_t>::max();

	std::vector<int32_t> weights(graph.size(), inf);
	std::vector<bool> visited(graph.size(), false);
	weights[0] = 0;

	int32_t min_index, min;
	do
	{
		min_index = min = inf;

		for (int32_t j = 0; j < graph.size(); j++)
		{
			if (!visited[j] && weights[j] < min)
			{
				min = weights[j];
				min_index = j;
			}
		}

		if (min_index != inf)
		{
			for (int32_t j = 0; j < graph[min_index].size(); j++)
			{
				int32_t id = graph[min_index][j];
				if (id >= 0)
				{
					int32_t t = min + 1; // 1 distance may be here...
					if (t < weights[id]) weights[id] = t;
				}
			}
			visited[min_index] = true;
		}
	} while (min_index != inf);

	for (int32_t j = 0; j < graph.size(); j++)
	{
		if (j == 0) Nodes[j][0].push_back(graph[0].size());
		else
		{
			auto& current_relay = Nodes[j][0];

			for (int32_t k = 0; k < graph[j].size(); k++)
			{
				int32_t id = graph[j][k];
				if (id >= 0)
				{
					int32_t t = weights[j] - 1;
					if (t == weights[id]) current_relay.push_back(k);
				}
			}
		}
	}

	for (int32_t i = 1; i < graph.size(); i++)
	{

	}
	
	return true;
}

void RoutingTable::push_back(Node&& node)
{
	Nodes.push_back(std::move(node));
}

RoutingTable::Node& RoutingTable::operator[](int32_t node_id)
{
	return Nodes[node_id];
}
const RoutingTable::Node& RoutingTable::operator[](int32_t node_id) const
{
	return Nodes[node_id];
}

bool RoutingTable::IsValid() const
{
	return Nodes.size();
}

std::ostream& operator<<(std::ostream& os, const RoutingTable& rt)
{
	os << "[\n";
	for (const auto& row : rt.Nodes)
	{
		os << '[';
		for (int32_t i = 0; i < row.size() - 1; i++)
		{
			const auto& cell = row[i];
			if (cell.size() > 1)
			{
				os << '[';
				for (int32_t j = 0; j < cell.size() - 1; j++)
					os << cell[j] << ", ";
				os << cell[cell.size() - 1];
				os << "], ";
			}
			else if (cell.size() == 1) os << cell[0] << ", ";
			else os << "x, ";
		}
		if (row.size())
		{
			const auto& cell = row[row.size() - 1];
			if (cell.size() > 1)
			{
				os << '[';
				for (int32_t j = 0; j < cell.size() - 1; j++)
					os << cell[j] << ", ";
				os << cell[cell.size() - 1];
				os << ']';
			}
			else if (cell.size() == 1) os << cell[0];
			else os << 'x';
		}
		os << "],\n";
	}
	os << ']';
	return os;
}
