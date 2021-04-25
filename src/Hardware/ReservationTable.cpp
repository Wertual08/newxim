#include "ReservationTable.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>



std::size_t ReservationTable::Size() const
{
	return Table.size();
}

void ReservationTable::Reserve(Connection dest_in, Connection dest_out)
{
	if (Reserved(dest_in, dest_out))
		throw std::runtime_error("Reservation error: Path already reserved.");
	static int cnt = 0;
	cnt++;
	for (std::size_t i = 0; i < Table.size(); i++)
	{
		if (Table[i].in == dest_in)
		{
			Table[i].out = dest_out;
			return;
		}
	}
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
		if (Table[i].out == dest_out) return true;
	return false;
}
bool ReservationTable::Reserved(Connection dest_out) const
{
	for (std::size_t i = 0; i < Table.size(); i++)
		if (Table[i].out == dest_out) return true;
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
