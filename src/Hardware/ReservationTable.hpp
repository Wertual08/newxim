#pragma once
#include <vector>
#include <ostream>



class ReservationTable
{
private:
	const std::int32_t PortsCount;
	std::vector<std::int32_t> Table;
	std::vector<std::int32_t> VirtualTable;

public:
	ReservationTable(std::int32_t ports, std::int32_t vcs = 10);

	void Reserve(std::int32_t port_in, std::int32_t vc_in, std::int32_t port_out, std::int32_t vc_out);
	void Release(std::int32_t port_in, std::int32_t vc_in);
	bool Reserved(std::int32_t port_out, std::int32_t vc_out) const;
	std::int32_t Reservation(std::int32_t port_in, std::int32_t vc_in) const;
	std::int32_t ReservationVC(std::int32_t port_in, std::int32_t vc_in) const;

	friend std::ostream& operator<<(std::ostream& os, const ReservationTable& table);
};
