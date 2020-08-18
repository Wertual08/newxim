#include "ReservationTable.hpp"



ReservationTable::ReservationTable(std::int32_t ports) :
	PortsCount(ports), Table(ports, -1)
{
}

void ReservationTable::Reserve(std::int32_t port_in, std::int32_t port_out)
{
	Table[port_in] = port_out;
}
void ReservationTable::Release(std::int32_t port_in)
{
	Table[port_in] = -1;
}
bool ReservationTable::Reserved(std::int32_t port_out) const
{
	for (std::int32_t r : Table) if (r == port_out) return true;
	return false;
}
std::int32_t ReservationTable::Reservation(std::int32_t port_in)
{
	return Table[port_in];
}

std::ostream& operator<<(std::ostream& os, const ReservationTable& table)
{
	for (std::int32_t p = 0; p < table.PortsCount; p++)
	{
		std::int32_t r = table.Table[p];
		os << p << " >> " << r << '\n';
	}
	return os;
}
