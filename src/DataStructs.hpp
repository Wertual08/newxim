#pragma once
#include <systemc.h>
#include <cstdint>



enum class FlitType : std::int32_t {
	None = 0b000,
	Head = 0b001,
	Body = 0b010, 
	Tail = 0b100
};
static FlitType operator|(FlitType l, FlitType r) {
	return static_cast<FlitType>(static_cast<std::int32_t>(l) | static_cast<std::int32_t>(r));
}
static FlitType operator&(FlitType l, FlitType r) {
	return static_cast<FlitType>(static_cast<std::int32_t>(l) & static_cast<std::int32_t>(r));
}
template<typename T>
static bool HasFlag(T flit, T flag) {
	return (flit & flag) == flag;
}

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