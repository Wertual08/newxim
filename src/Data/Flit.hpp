#pragma once
#include <systemc.h>
#include <cstdint>
#include "FlitType.hpp"



struct Flit {
	std::uint64_t id;
	int src_id = -1;
	int dst_id = -1;
	int port_in = -1;
	int port_out = -1;
	int vc_id = -1;							
	FlitType flit_type = FlitType::None;	
	int sequence_no = -1;					
	int sequence_length = -1;
	double timestamp = -1;					
	double accept_timestamp = - 1;
	int hop_no = -1;						

	inline bool operator==(const Flit& flit) const {
		return flit.id == id
			&& flit.src_id == src_id
			&& flit.dst_id == dst_id
			&& flit.flit_type == flit_type
			&& flit.vc_id == vc_id
			&& flit.sequence_no == sequence_no
			&& flit.sequence_length == sequence_length
			&& flit.timestamp == timestamp
			&& flit.hop_no == hop_no;
	}
	bool valid() const { return flit_type != FlitType::None; }
};

void sc_trace(sc_trace_file *&tf, const Flit &flit, std::string &name);
std::ostream &operator<<(std::ostream &os, const Flit &flit);