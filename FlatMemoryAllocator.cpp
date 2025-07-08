#include "FlatMemoryAllocator.h"
#include <string>
#include <memory>

FlatMemoryAllocator::FlatMemoryAllocator(uint16_t size) {
	memory.reserve(size);
	initializeMemory();
}

void FlatMemoryAllocator::initializeMemory() {
	// Initialize the memory with a single free block covering the entire size
	fill(memory.begin(), memory.end(), '.');
	fill(allocationMap.begin(), allocationMap.end(), false);
}

