#pragma once
#include <cstdint>
#include <utility>
#include <ostream>



struct Connection
{
	std::int32_t port = -1;
	std::int32_t vc = -1;

	bool valid() const { return port >= 0 && vc >= 0; }
	bool operator==(Connection con) const { return port == con.port && vc == con.vc; }
	bool operator!=(Connection con) const { return port != con.port || vc != con.vc; }
	bool operator<(Connection con) const 
	{ 
		if (port < con.port) return true;
		return vc < con.vc;
	}

	friend std::ostream& operator<<(std::ostream& os, const Connection& con)
	{
		return os << '(' << con.port << ':' << con.vc << ')';
	}
};

struct PairConnection
{
	Connection in, out;

	bool operator==(const PairConnection& con) const { return in == con.in && out == con.out; }
	bool operator!=(const PairConnection& con) const { return in != con.in || out != con.out; }
	bool operator<(const PairConnection& con) const { return in < con.in; }
};