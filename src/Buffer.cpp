/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the buffer
 */

#include "Buffer.h"
#include <cassert>



void Buffer::SaveOccupancyAndTime()
{
	previous_occupancy = buffer.size();
	hold_time = (sc_time_stamp().to_double() / GlobalParams::clock_period_ps) - last_event;
	last_event = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
}
void Buffer::UpdateMeanOccupancy()
{
	double current_time = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
	if (current_time - GlobalParams::reset_time < GlobalParams::stats_warm_up_time) return;

	mean_occupancy = mean_occupancy * (hold_time_sum / (hold_time_sum + hold_time)) +
		(1.0 / (hold_time_sum + hold_time)) * hold_time * buffer.size();

	hold_time_sum += hold_time;
}

Buffer::Buffer()
{
	SetMaxBufferSize(GlobalParams::buffer_depth);
	max_occupancy = 0;
	hold_time = 0.0;
	last_event = 0.0;
	hold_time_sum = 0.0;
	previous_occupancy = 0;
	mean_occupancy = 0.0;
	true_buffer = true;
	full_cycles_counter = 0;
	last_front_flit_seq = NOT_VALID;
	deadlock_detected = false;
}

void Buffer::SetMaxBufferSize(uint32_t bms)
{
	assert(bms > 0);

	max_buffer_size = bms;
}
uint32_t Buffer::GetMaxBufferSize() const
{
	return max_buffer_size;
}
uint32_t Buffer::GetCurrentFreeSlots() const
{
	return (GetMaxBufferSize() - Size());
}

bool Buffer::IsFull() const
{
	return buffer.size() == max_buffer_size;
}
bool Buffer::IsEmpty() const
{
	return buffer.size() == 0;
}

void Buffer::Drop(const Flit& flit) const
{
	assert(false);
}
void Buffer::Empty() const
{
	assert(false);
}
void Buffer::Disable()
{
	true_buffer = false;
}

void Buffer::Push(const Flit& flit)
{
	SaveOccupancyAndTime();

	if (IsFull()) Drop(flit);
	else buffer.push(flit);

	UpdateMeanOccupancy();

	if (max_occupancy < buffer.size())
		max_occupancy = buffer.size();
}
Flit Buffer::Pop()
{
	Flit f;

	SaveOccupancyAndTime();

	if (IsEmpty()) Empty();
	else 
	{
		f = buffer.front();
		buffer.pop();
	}

	UpdateMeanOccupancy();

	return f;
}
Flit Buffer::Front() const
{
	Flit f;

	if (IsEmpty()) Empty();
	else f = buffer.front();

	return f;
}
uint32_t Buffer::Size() const
{
	return buffer.size();
}

void Buffer::ShowStats(std::ostream& out) const
{
	if (true_buffer) out << "\t" << mean_occupancy << "\t" << max_occupancy;
	else out << "\t\t";
}
void Buffer::Print() const
{
	std::queue<Flit> m = buffer;

	std::string bstr = "";


	char  t[] = "HBT";

	cout << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << "\t";
	cout << label << " QUEUE *[";
	while (!(m.empty()))
	{
		Flit f = m.front();
		m.pop();
		cout << bstr << t[f.flit_type] << f.sequence_no << "(" << f.dst_id << ") | ";
	}
	cout << "]*" << endl;
	cout << endl;
}

void Buffer::SetLabel(std::string l)
{
	label = l;
}
std::string Buffer::GetLabel() const
{
	return label;
}
