#pragma once
#include <string>
#include <cstddef> // for size_t
using namespace std;
class MemoryAllocator
{
public:
	MemoryAllocator() = default;
	virtual ~MemoryAllocator() = default;
	virtual void* Allocate(uint16_t size) = 0;
	virtual void Deallocate(void* ptr) = 0;
	virtual string visualizeMemory() const = 0;
	virtual char* getMemoryBase() = 0;
	virtual uint16_t getBlockSizeAt(uint16_t index) const = 0;
	virtual size_t getTotalExtFrag() const = 0;
	virtual uint16_t getMaxSize() const = 0;
};

