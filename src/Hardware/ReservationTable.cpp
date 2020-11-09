#include "ReservationTable.hpp"



ReservationTable::ReservationTable(std::int32_t ports, std::int32_t vcs) :
	PortsCount(ports), Table(ports * vcs, -1), VirtualTable(ports * vcs, -1)
{
}

void ReservationTable::Reserve(std::int32_t port_in, std::int32_t vc_in, std::int32_t port_out, std::int32_t vc_out)
{
	Table[port_in + vc_in * PortsCount] = port_out;
	VirtualTable[port_in + vc_in * PortsCount] = vc_out;
}
void ReservationTable::Release(std::int32_t port_in, std::int32_t vc_in)
{
	Table[port_in + vc_in * PortsCount] = -1;
	VirtualTable[port_in + vc_in * PortsCount] = -1;
}
bool ReservationTable::Reserved(std::int32_t port_out, std::int32_t vc_out) const
{
	for (std::size_t i = 0; i < Table.size(); i++)
		if (Table[i] == port_out && VirtualTable[i] == vc_out) return true;
	return false;
}
std::int32_t ReservationTable::Reservation(std::int32_t port_in, std::int32_t vc_in) const
{
	return Table[port_in + vc_in * PortsCount];
}
std::int32_t ReservationTable::ReservationVC(std::int32_t port_in, std::int32_t vc_in) const
{
	return VirtualTable[port_in + vc_in * PortsCount];
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
