#pragma once
#include <vector>
#include <ostream>



class ReservationTable
{
private:
	const int32_t PortsCount;
	std::vector<int32_t> Table;

public:
	ReservationTable(int32_t ports);

	void Reserve(int32_t port_in, int32_t port_out);
	void Release(int32_t port_in);
	bool Reserved(int32_t port_out) const;
	int32_t Reservation(int32_t port_in);

	friend std::ostream& operator<<(std::ostream& os, const ReservationTable& table);
};
