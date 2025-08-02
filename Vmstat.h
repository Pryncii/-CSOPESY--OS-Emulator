#pragma once
#include <cstdint>
#include <iostream>
#include <memory>
#include "Process.h"
#include "PagingAllocator.h"
class Vmstat
{
public:
	Vmstat(shared_ptr<PagingAllocator> PagingAllocator);

private:
	shared_ptr<PagingAllocator> PagingAllocator;
	uint16_t totalMemory; // Total memory in bytes
	uint16_t usedMemory;  // Used memory in bytes
	uint16_t freeMemory;  // Free memory in bytes
	uint16_t idleTicks;
	uint16_t activeTicks;
	uint16_t totalTicks;
	uint16_t pagedIn;
	uint16_t pagedOut;
};

