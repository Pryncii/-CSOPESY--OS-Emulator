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
	
};

