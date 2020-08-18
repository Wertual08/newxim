#pragma once
#include <vector>
#include <ostream>



class ReservationTable
{
private:
	const std::int32_t PortsCount;
	std::vector<std::int32_t> Table;

public:
	ReservationTable(std::int32_t ports);

	void Reserve(std::int32_t port_in, std::int32_t port_out);
	void Release(std::int32_t port_in);
	bool Reserved(std::int32_t port_out) const;
	std::int32_t Reservation(std::int32_t port_in);

	friend std::ostream& operator<<(std::ostream& os, const ReservationTable& table);
};
