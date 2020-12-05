#include "FlitTracer.hpp"
#include <iomanip>



FlitTracer::FlitTracer(const SimulationTimer& timer, double story_start, double story_end) :
	Timer(timer), StoryStart(story_start), StoryEnd(story_end)
{
}

void FlitTracer::Register(Flit& flit)
{
	if ((StoryStart < 0 || flit.accept_timestamp >= StoryStart) && 
		(StoryEnd < 0 || flit.accept_timestamp <= StoryEnd))
	{
		flit.id = IDOffset + FlitHistory.size();
		FlitHistory.push_back(std::make_pair(flit, std::vector<Location>()));
	}
	else flit.id = IDOffset++;
}
void FlitTracer::Remember(const Flit& flit, std::int32_t id)
{
	if (flit.id - IDOffset >= FlitHistory.size() || flit.id < IDOffset) return;
	FlitHistory[flit.id - IDOffset].second.push_back({ id, flit.dir_in, flit.vc_id, Timer.SimulationTime() });
}

std::ostream& operator<<(std::ostream& os, const FlitTracer& tracer)
{
	for (const auto& story : tracer.FlitHistory)
	{
		const auto& f = story.first;
		os << '[' << std::left << std::setw(6) << f.id << ']';
		if (HasFlag(f.flit_type, FlitType::Head)) os << 'H';
		if (HasFlag(f.flit_type, FlitType::Body)) os << 'B';
		if (HasFlag(f.flit_type, FlitType::Tail)) os << 'T';
		os << f.sequence_no << '(' << f.src_id << "->" << f.dst_id << "): \t";
		for (const auto& loc : story.second)
		{
			os << '{' << loc.timestamp << '|' << loc.port << ':' << loc.vc << "->" << loc.id << "} ";
		}
		os << '\n';
	}
	return os;
}
