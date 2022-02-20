#pragma once
#include <queue>
#include "Data/Flit.hpp"



class Buffer 
{
private:
	std::size_t max_buffer_size;
	std::queue<Flit> buffer;

public:

	void Reserve(std::size_t bms);			
	std::size_t GetCapacity() const;			
	std::size_t GetFreeSlots() const;

	bool Full() const;
	bool Empty() const;					

	void Clear();
	void Push(const Flit& flit);
	Flit Pop();
	Flit Front() const;
	std::int32_t Size() const;

	double GetOldest() const;
	double GetOldestAccepted() const;
	double GetLoad() const;

	friend std::ostream& operator<<(std::ostream& os, const Buffer& b);
};
