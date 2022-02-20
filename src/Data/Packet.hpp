#pragma once
#include <systemc.h>
#include <cstdint>



struct Packet  {
	std::int32_t src_id;
	std::int32_t dst_id;
	std::int32_t vc_id;
	double timestamp;
	std::int32_t size;
	std::int32_t flit_left;	

	Packet() {
		src_id = -1;
		dst_id = -1;
		vc_id = -1;
		timestamp = -1;
		size = -1;
		flit_left = -1;
	}
	Packet(std::int32_t s, std::int32_t d, std::int32_t vc, double ts, std::int32_t sz) {
		src_id = s;
		dst_id = d;
		vc_id = vc;
		timestamp = ts;
		size = sz;
		flit_left = sz;
	}
};
