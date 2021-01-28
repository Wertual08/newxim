#pragma once
#include <vector>
#include <ostream>
#include "Hardware/Connection.hpp"



class ReservationTable
{
private:
	std::vector<PairConnection> Table;

public:
	std::size_t Size() const;

	void Reserve(Connection dest_in, Connection dest_out);
	void Release(Connection dest_in);
	bool Reserved(Connection dest_in, Connection dest_out) const;
	Connection operator[](Connection dest_in) const;

	friend std::ostream& operator<<(std::ostream& os, const ReservationTable& table);
};
