/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global params needed by Noxim
 * to forward configuration to every sub-block
 */
#pragma once
#include <systemc.h>
#include <tlm>

#include "DataStructs.h"
#include <iomanip>
#include <sstream>

#ifdef DEBUG

#define LOG (std::cout << std::setw(7) << left << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << " " << name() << "::" << __func__<< "() --> ")

#else
template <class cT, class traits = std::char_traits<cT> >
class basic_nullbuf : public std::basic_streambuf<cT, traits> {
	typename traits::int_type overflow(typename traits::int_type c)
	{
		return traits::not_eof(c); // indicate success
	}
};

template <class cT, class traits = std::char_traits<cT> >
class basic_onullstream : public std::basic_ostream<cT, traits> {
public:
	basic_onullstream() :
		std::basic_ios<cT, traits>(&m_sbuf),
		std::basic_ostream<cT, traits>(&m_sbuf)
	{
		// note: the original code is missing the required this->
		this->init(&m_sbuf);
	}

private:
	basic_nullbuf<cT, traits> m_sbuf;
};

typedef basic_onullstream<char> onullstream;
typedef basic_onullstream<wchar_t> wonullstream;

static onullstream LOG;

#endif

// Output overloading

inline ostream& operator <<(ostream& os, const Flit& flit)
{
	if (GlobalParams::verbose_mode == VERBOSE_HIGH) 
	{
		os << "### FLIT ###" << endl;
		os << "Source Tile[" << flit.src_id << "]" << endl;
		os << "Destination Tile[" << flit.dst_id << "]" << endl;
		switch (flit.flit_type) {
		case FLIT_TYPE_HEAD:
			os << "Flit Type is HEAD" << endl;
			break;
		case FLIT_TYPE_BODY:
			os << "Flit Type is BODY" << endl;
			break;
		case FLIT_TYPE_TAIL:
			os << "Flit Type is TAIL" << endl;
			break;
		}
		os << "Sequence no. " << flit.sequence_no << endl;
		os << "Payload printing not implemented (yet)." << endl;
		os << "Unix timestamp at packet generation " << flit.
			timestamp << endl;
		os << "Total number of hops from source to destination is " <<
			flit.hop_no << endl;
	}
	else {
		os << "(";
		switch (flit.flit_type) {
		case FLIT_TYPE_HEAD:
			os << "H";
			break;
		case FLIT_TYPE_BODY:
			os << "B";
			break;
		case FLIT_TYPE_TAIL:
			os << "T";
			break;
		}

		os << flit.sequence_no << ", " << flit.src_id << "->" << flit.dst_id << " VC " << flit.vc_id << ")";
	}

	return os;
}

inline ostream& operator<<(ostream& os, const ChannelStatus& status)
{
	char msg;
	if (status.available)
		msg = 'A';
	else
		msg = 'N';
	os << msg << "(" << status.free_slots << ")";
	return os;
}

// Trace overloading

inline void sc_trace(sc_trace_file*& tf, const Flit& flit, std::string& name)
{
	sc_trace(tf, flit.src_id, name + ".src_id");
	sc_trace(tf, flit.dst_id, name + ".dst_id");
	sc_trace(tf, flit.sequence_no, name + ".sequence_no");
	sc_trace(tf, flit.timestamp, name + ".timestamp");
	sc_trace(tf, flit.hop_no, name + ".hop_no");
}

inline void sc_trace(sc_trace_file*& tf, const ChannelStatus& bs, std::string& name)
{
	sc_trace(tf, bs.free_slots, name + ".free_slots");
	sc_trace(tf, bs.available, name + ".available");
}

// Misc common functions
inline void printMap(std::string label, const std::map<std::string, double>& m, std::ostream& out)
{
	out << label << " = [" << endl;
	for (std::map<std::string, double>::const_iterator i = m.begin(); i != m.end(); i++)
		out << "\t" << std::scientific << i->second << "\t % " << i->first << endl;

	out << "];" << endl;
}

template<typename T> 
std::string i_to_string(const T& t) 
{
	std::stringstream s;
	s << t;
	return s.str();
}