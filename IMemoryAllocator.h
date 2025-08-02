#pragma once
#include <string>
#include <cstddef> // for size_t
#include "Process.h"
using namespace std;
class IMemoryAllocator
{
public:
	IMemoryAllocator() = default;
	virtual ~IMemoryAllocator() = default;
	virtual void* Allocate(shared_ptr<Process> process) = 0;
	virtual void Deallocate(shared_ptr<Process> process) = 0;
	virtual void visualizeMemory() const = 0;
	virtual char* getMemoryBase() = 0;
	virtual uint16_t getBlockSizeAt(uint16_t index) const = 0;
	virtual size_t getTotalExtFrag() const = 0;
	virtual uint16_t getMaxSize() const = 0;
	virtual unordered_map<uint16_t, uint16_t> getFrameMap() const = 0;
	virtual size_t getMemoryUsed() const = 0;
};

