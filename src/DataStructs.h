/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the top-level of Noxim
 */

#pragma once
#include <systemc.h>
#include "GlobalParams.h"

// FlitType -- Flit type enumeration
enum FlitType 
{
	FLIT_TYPE_HEAD,
	FLIT_TYPE_BODY, 
	FLIT_TYPE_TAIL
};

// Payload -- Payload definition
struct Payload 
{
	sc_uint<32> data;	// Bus for the data to be exchanged

	inline bool operator==(const Payload& payload) const 
	{
		return (payload.data == data);
	}
};

// Packet -- Packet definition
struct Packet 
{
	int src_id;
	int dst_id;
	int vc_id;
	double timestamp;		// SC timestamp at packet generation
	int size;
	int flit_left;		// Number of remaining flits inside the packet
	bool use_low_voltage_path;

	// Constructors
	Packet() { }

	Packet(const int s, const int d, const int vc, const double ts, const int sz) 
	{
		make(s, d, vc, ts, sz);
	}

	void make(const int s, const int d, const int vc, const double ts, const int sz) 
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
	int32_t hop_no;
	int32_t current_id;
	int32_t src_id;
	int32_t dst_id;
	int32_t dir_in;			// direction from which the packet comes from
	int32_t vc_id;
	int32_t sequence_length;
};

struct ChannelStatus 
{
	int free_slots;		// occupied buffer slots
	bool available;		// 
	inline bool operator ==(const ChannelStatus& bs) const 
	{
		return (free_slots == bs.free_slots && available == bs.available);
	};
};

// Flit -- Flit definition
struct Flit 
{
	int src_id;
	int dst_id;
	int vc_id;				// Virtual Channel
	FlitType flit_type;		// The flit type (FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL)
	int sequence_no;		// The sequence number of the flit inside the packet
	int sequence_length;
	Payload payload;		// Optional payload
	double timestamp;		// Unix timestamp at packet generation
	int hop_no;				// Current number of hops from source to destination
	bool use_low_voltage_path;

	inline bool operator==(const Flit& flit) const 
	{
		return flit.src_id == src_id 
			&& flit.dst_id == dst_id
			&& flit.flit_type == flit_type
			&& flit.vc_id == vc_id
			&& flit.sequence_no == sequence_no
			&& flit.sequence_length == sequence_length
			&& flit.payload == payload 
			&& flit.timestamp == timestamp
			&& flit.hop_no == hop_no
			&& flit.use_low_voltage_path == use_low_voltage_path;
	}
};


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
