#include "ReservationTable.hpp"
#include <algorithm>



ReservationTable::ReservationTable(std::int32_t ports, std::int32_t vcs) 
{
}

void ReservationTable::Reserve(Connection dest_in, Connection dest_out)
{
	Table.push_back({ dest_in, dest_out });
}
void ReservationTable::Release(Connection dest_in)
{
	std::size_t s = 0;
	for (std::size_t i = 0; i < Table.size(); i++)
		if (Table[i].in != dest_in) Table[s++] = Table[i];

	Table.resize(s);
}
bool ReservationTable::Reserved(Connection dest_in, Connection dest_out) const
{
	for (std::size_t i = 0; i < Table.size(); i++)
		if (Table[i].in.port == dest_in.port || Table[i].out.port == dest_out.port) return true;
	return false;
}
Connection ReservationTable::operator[](Connection dest_in) const
{
	for (const auto& node : Table)
		if (node.in == dest_in)
			return node.out;
	return Connection();
}

std::ostream& operator<<(std::ostream& os, const ReservationTable& table)
{
	auto t = table.Table;
	for (const auto& node : t)
	{
		os << node.in << " >> " << node.out << '\n';
	}
	return os;
}
