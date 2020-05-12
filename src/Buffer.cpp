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



Buffer::Buffer()
{
	SetMaxBufferSize(GlobalParams::buffer_depth);
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

void Buffer::Push(const Flit& flit)
{
	if (IsFull()) Drop(flit);
	else buffer.push(flit);
}
Flit Buffer::Pop()
{
	Flit f;

	if (IsEmpty()) Empty();
	else 
	{
		f = buffer.front();
		buffer.pop();
	}

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

double Buffer::GetOldest() const
{
	auto copy = buffer;
	double result = copy.front().timestamp;
	copy.pop();
	while (!copy.empty())
	{
		if (copy.front().timestamp < result) result = copy.front().timestamp;
		copy.pop();
	}
	return result;
}
double Buffer::GetLoad() const
{
	return (double)Size() / (double)GetMaxBufferSize();
}

void Buffer::SetLabel(std::string l)
{
	label = l;
}
std::string Buffer::GetLabel() const
{
	return label;
}

std::ostream& operator<<(std::ostream& os, const Buffer& b)
{
	std::queue<Flit> m = b.buffer;

	char t[] = "HBT";

	os << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << "\t";
	os << b.label << " QUEUE *[";
	while (!m.empty())
	{
		Flit f = m.front();
		m.pop();
		os << t[f.flit_type] << f.sequence_no << "(" << f.src_id << "->" << f.dst_id << ") | ";
	}
	return os << "]*\n";
}
