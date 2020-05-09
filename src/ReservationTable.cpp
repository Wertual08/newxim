#include "ReservationTable.h"



ReservationTable::ReservationTable(int32_t ports) :
	PortsCount(ports), Table(ports, -1)
{
}

void ReservationTable::Reserve(int32_t port_in, int32_t port_out)
{
	Table[port_in] = port_out;
}
void ReservationTable::Release(int32_t port_in)
{
	Table[port_in] = -1;
}
bool ReservationTable::Reserved(int32_t port_out) const
{
	for (int32_t r : Table) if (r == port_out) return true;
	return false;
}
int32_t ReservationTable::Reservation(int32_t port_in)
{
	return Table[port_in];
}

std::ostream& operator<<(std::ostream& os, const ReservationTable& table)
{
	for (int32_t p = 0; p < table.PortsCount; p++)
	{
		int32_t r = table.Table[p];
		os << p << " >> " << r << '\n';
	}
	return os;
}
