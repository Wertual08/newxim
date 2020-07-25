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

#include "DataStructs.hpp"
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

// Trace overloading

inline void sc_trace(sc_trace_file*& tf, const Flit& flit, std::string& name)
{
	sc_trace(tf, flit.src_id, name + ".src_id");
	sc_trace(tf, flit.dst_id, name + ".dst_id");
	sc_trace(tf, flit.sequence_no, name + ".sequence_no");
	sc_trace(tf, flit.timestamp, name + ".timestamp");
	sc_trace(tf, flit.hop_no, name + ".hop_no");
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