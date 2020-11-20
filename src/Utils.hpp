#pragma once
#include <systemc.h>
#include <tlm>

#include "DataStructs.hpp"
#include <iomanip>
#include <sstream>
#include <map>


inline void sc_trace(sc_trace_file*& tf, const Flit& flit, std::string& name)
{
	sc_trace(tf, flit.src_id, name + ".src_id");
	sc_trace(tf, flit.dst_id, name + ".dst_id");
	sc_trace(tf, flit.sequence_no, name + ".sequence_no");
	sc_trace(tf, flit.timestamp, name + ".timestamp");
	sc_trace(tf, flit.hop_no, name + ".hop_no");
}