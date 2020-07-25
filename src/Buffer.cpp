/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the buffer
 */

#include "Buffer.hpp"
#include <cassert>



Buffer::Buffer()
{
}

void Buffer::SetMaxBufferSize(int32_t bms)
{
	assert(bms > 0);

	max_buffer_size = bms;
}
int32_t Buffer::GetMaxBufferSize() const
{
	return max_buffer_size;
}
int32_t Buffer::GetCurrentFreeSlots() const
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
int32_t Buffer::Size() const
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

std::ostream& operator<<(std::ostream& os, const Buffer& b)
{
	std::queue<Flit> m = b.buffer;

	char t[] = "HBT";

	os << '[';
	while (m.size() > 1)
	{
		Flit f = m.front();
		m.pop();
		os << t[f.flit_type] << f.sequence_no << '(' << f.src_id << "->" << f.dst_id << ") | ";
	}
	if (!m.empty())
	{
		Flit f = m.front();
		os << t[f.flit_type] << f.sequence_no << '(' << f.src_id << "->" << f.dst_id << ')';
	}
	return os << ']';
}
