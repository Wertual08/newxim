#pragma once
#include <cstdint>
#include <ostream>



// FlitType -- Flit type enumeration
enum class FlitType : std::int32_t
{
	None = 0b000,
	Head = 0b001,
	Body = 0b010, 
	Tail = 0b100
};
static FlitType operator|(FlitType l, FlitType r)
{
	return static_cast<FlitType>(static_cast<std::int32_t>(l) | static_cast<std::int32_t>(r));
}
static FlitType operator&(FlitType l, FlitType r)
{
	return static_cast<FlitType>(static_cast<std::int32_t>(l) & static_cast<std::int32_t>(r));
}
template<typename T>
static bool HasFlag(T flit, T flag)
{
	return (flit & flag) == flag;
}

// Packet -- Packet definition
struct Packet 
{
	std::int32_t src_id;
	std::int32_t dst_id;
	std::int32_t vc_id;
	double timestamp;		// SC timestamp at packet generation
	std::int32_t size;
	std::int32_t flit_left;		// Number of remaining flits inside the packet
	bool use_low_voltage_path;

	Packet() 
	{
		src_id = -1;
		dst_id = -1;
		vc_id = -1;
		timestamp = -1;
		size = -1;
		flit_left = -1;
		use_low_voltage_path = false;
	}
	Packet(std::int32_t s, std::int32_t d, std::int32_t vc, double ts, std::int32_t sz)
	{
		src_id = s;
		dst_id = d;
		vc_id = vc;
		timestamp = ts;
		size = sz;
		flit_left = sz;
		use_low_voltage_path = false;
	}
};

// RouteData -- data required to perform routing
struct RouteData 
{
	std::int32_t hop_no;
	std::int32_t current_id;
	std::int32_t src_id;
	std::int32_t dst_id;
	std::int32_t dir_in;			// direction from which the packet comes from
	std::int32_t vc_id;
	std::int32_t sequence_length;
};

// Flit -- Flit definition
struct Flit 
{
	int src_id = -1;
	int dst_id = -1;
	int vc_id = -1;							// Virtual Channel
	FlitType flit_type = FlitType::None;	// The flit type (FlitType::Head, FlitType::Body, FlitType::Tail)
	int sequence_no = -1;					// The sequence number of the flit inside the packet
	int sequence_length = -1;
	double timestamp = -1;					// Unix timestamp at packet generation
	double accept_timestamp = - 1;
	int hop_no = -1;						// Current number of hops from source to destination
	bool use_low_voltage_path = false;

	inline bool operator==(const Flit& flit) const 
	{
		return flit.src_id == src_id 
			&& flit.dst_id == dst_id
			&& flit.flit_type == flit_type
			&& flit.vc_id == vc_id
			&& flit.sequence_no == sequence_no
			&& flit.sequence_length == sequence_length
			&& flit.timestamp == timestamp
			&& flit.hop_no == hop_no
			&& flit.use_low_voltage_path == use_low_voltage_path;
	}
};

inline std::ostream& operator <<(std::ostream& os, const Flit& flit)
{
	os << '(';
	if (HasFlag(flit.flit_type, FlitType::Head)) os << 'H';
	if (HasFlag(flit.flit_type, FlitType::Body)) os << 'B';
	if (HasFlag(flit.flit_type, FlitType::Tail)) os << 'T';
	return os << flit.sequence_no << ", " << flit.src_id << "->" << flit.dst_id << " VC " << flit.vc_id << ')';
}


struct PowerBreakdownEntry
{
	std::string label;
	double value;
};


enum
{
	BUFFER_PUSH_PWR_D,
	BUFFER_POP_PWR_D,
	BUFFER_FRONT_PWR_D,
	BUFFER_TO_TILE_PUSH_PWR_D,
	BUFFER_TO_TILE_POP_PWR_D,
	BUFFER_TO_TILE_FRONT_PWR_D,
	BUFFER_FROM_TILE_PUSH_PWR_D,
	BUFFER_FROM_TILE_POP_PWR_D,
	BUFFER_FROM_TILE_FRONT_PWR_D,
	ANTENNA_BUFFER_PUSH_PWR_D,
	ANTENNA_BUFFER_POP_PWR_D,
	ANTENNA_BUFFER_FRONT_PWR_D,
	ROUTING_PWR_D,
	SELECTION_PWR_D,
	CROSSBAR_PWR_D,
	LINK_R2R_PWR_D,
	LINK_R2H_PWR_D,
	NI_PWR_D,
	NO_BREAKDOWN_ENTRIES_D
};

enum
{
	TRANSCEIVER_RX_PWR_BIASING,
	TRANSCEIVER_TX_PWR_BIASING,
	BUFFER_ROUTER_PWR_S,
	BUFFER_TO_TILE_PWR_S,
	BUFFER_FROM_TILE_PWR_S,
	ANTENNA_BUFFER_PWR_S,
	LINK_R2H_PWR_S,
	ROUTING_PWR_S,
	SELECTION_PWR_S,
	CROSSBAR_PWR_S,
	NI_PWR_S,
	TRANSCEIVER_RX_PWR_S,
	TRANSCEIVER_TX_PWR_S,
	NO_BREAKDOWN_ENTRIES_S
};

struct PowerBreakdown
{
	int size;
	PowerBreakdownEntry breakdown[NO_BREAKDOWN_ENTRIES_D + NO_BREAKDOWN_ENTRIES_S];
};
