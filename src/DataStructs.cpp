#include "DataStructs.hpp"
#include <systemc.h>



void sc_trace(sc_trace_file *&tf, const Flit &flit, std::string &name)
{
	sc_trace(tf, flit.src_id, name + ".src_id");
	sc_trace(tf, flit.dst_id, name + ".dst_id");
	sc_trace(tf, flit.sequence_no, name + ".sequence_no");
	sc_trace(tf, flit.timestamp, name + ".timestamp");
	sc_trace(tf, flit.hop_no, name + ".hop_no");
};
std::ostream &operator<<(std::ostream &os, const Flit &flit)
{
	os << '(';
	if (HasFlag(flit.flit_type, FlitType::Head)) os << 'H';
	if (HasFlag(flit.flit_type, FlitType::Body)) os << 'B';
	if (HasFlag(flit.flit_type, FlitType::Tail)) os << 'T';
	return os << flit.sequence_no << ", " << flit.src_id << "->" << flit.dst_id << " VC " << flit.vc_id << ')';
}