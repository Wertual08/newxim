#pragma once
#include <vector>
#include <ostream>
#include "Hardware/Connection.hpp"



class ReservationTable
{
private:
	std::vector<PairConnection> Table;

public:
	ReservationTable(std::int32_t ports, std::int32_t vcs = 10);

	void Reserve(Connection dest_in, Connection dest_out);
	void Release(Connection dest_in);
	bool Reserved(Connection dest_in, Connection dest_out) const;
	Connection operator[](Connection dest_in) const;

	friend std::ostream& operator<<(std::ostream& os, const ReservationTable& table);
};
