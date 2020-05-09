#pragma once
#include <queue>
#include "DataStructs.h"



class Buffer 
{
private:
	bool true_buffer;
	bool deadlock_detected;

	int32_t full_cycles_counter;
	int32_t last_front_flit_seq;

	std::string label;

	int32_t max_buffer_size;

	std::queue<Flit> buffer;
	mutable Flit TEMP_SHITY_FLIT;
	mutable int TEMP_SHITY_COUNTER;

	uint32_t max_occupancy;
	double hold_time, last_event, hold_time_sum;
	double mean_occupancy;
	int32_t previous_occupancy;

	void SaveOccupancyAndTime();
	void UpdateMeanOccupancy();

public:
	Buffer();
	virtual ~Buffer() {
	} 

	void SetMaxBufferSize(uint32_t bms);			// Set buffer max size (in flits)
	uint32_t GetMaxBufferSize() const;				// Get max buffer size
	uint32_t GetCurrentFreeSlots() const;			// free buffer slots

	bool IsFull() const;							// Returns true if buffer is full
	bool IsEmpty() const;							// Returns true if buffer is empty

	virtual void Drop(const Flit& flit) const;		// Called by Push() when buffer is full
	virtual void Empty() const;						// Called by Pop() when buffer is empty
	void Disable();

	void Push(const Flit& flit);					// Push a flit. Calls Drop method if buffer is full
	Flit Pop();										// Pop a flit
	Flit Front() const;								// Return a copy of the first flit in the buffer
	uint32_t Size() const;

	void ShowStats(std::ostream& out) const;
	void Print() const;

	void SetLabel(std::string);
	std::string GetLabel() const;

	friend std::ostream& operator<<(std::ostream& os, const Buffer& b);
};
