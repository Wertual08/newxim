#pragma once
#include <queue>
#include "DataStructs.hpp"



class Buffer 
{
private:
	std::int32_t max_buffer_size;
	std::queue<Flit> buffer;

public:
	Buffer();
	virtual ~Buffer() {
	} 

	void SetMaxBufferSize(std::int32_t bms);				// Set buffer max size (in flits)
	std::int32_t GetMaxBufferSize() const;				// Get max buffer size
	std::int32_t GetCurrentFreeSlots() const;			// free buffer slots

	bool IsFull() const;							// Returns true if buffer is full
	bool IsEmpty() const;							// Returns true if buffer is empty

	virtual void Drop(const Flit& flit) const;		// Called by Push() when buffer is full
	virtual void Empty() const;						// Called by Pop() when buffer is empty
	void Disable();

	void Push(const Flit& flit);					// Push a flit. Calls Drop method if buffer is full
	Flit Pop();										// Pop a flit
	Flit Front() const;								// Return a copy of the first flit in the buffer
	std::int32_t Size() const;

	double GetOldest() const;
	double GetLoad() const;

	friend std::ostream& operator<<(std::ostream& os, const Buffer& b);
};
