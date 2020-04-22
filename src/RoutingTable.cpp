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

RoutingTable::RoutingTable() : Valid(false)
{
}

bool RoutingTable::Load(const std::string& path)
{
	std::ifstream fin(path, std::ios::in);

	if (!fin) return false;

	Nodes.clear();
	Valid = false;

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

	return Valid = true;
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
	return Valid;
}
