#include "FlatMemoryAllocator.h"
#include <string>
#include <memory>

FlatMemoryAllocator::FlatMemoryAllocator(uint16_t size) {
	maxSize = size;
	allocatedSize = 0;
	memory.reserve(maxSize);
	initializeMemory();
}

FlatMemoryAllocator::~FlatMemoryAllocator() {
	memory.clear();
}

void* FlatMemoryAllocator::Allocate(uint16_t size) {
	for (uint16_t i = 0; i < maxSize - size + 1; ++i) {
		if (!allocationMap[i] && canAllocateAt(i, size)) {
			allocateAt(i, size);
			return &memory[i];
		}
	}

	return nullptr;
}

void FlatMemoryAllocator::Deallocate(void* ptr) {
	uint16_t index = static_cast<char*>(ptr) - &memory[0];
	if (allocationMap[index]) {
		deallocateAt(index);
	}
}

string FlatMemoryAllocator::visualizeMemory() const {
	return string(memory.begin(), memory.end());
}

void FlatMemoryAllocator::initializeMemory() {
	// Initialize the memory with a single free block covering the entire size
	fill(memory.begin(), memory.end(), '.');

	allocationMap.clear();
	for (uint16_t i = 0; i < maxSize; ++i) {
		allocationMap[i] = false; // mark each byte index as free
	}
}

bool FlatMemoryAllocator::canAllocateAt(uint16_t index, uint16_t size) const {
	return (index + size <= maxSize);
}

void FlatMemoryAllocator::allocateAt(uint16_t index, uint16_t size) {
	blockSizes[index] = size; // save size for deallocation purposes
	for (uint16_t i = index; i < index + size; ++i) { // [index, index+size)
		allocationMap[i] = true;  // Mark as allocated
	}
	allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(uint16_t index) {
	uint16_t size = blockSizes[index];
	for (uint16_t i = index; i < index + size; ++i) {
		allocationMap[i] = false;
	}
	blockSizes.erase(index);
	allocatedSize -= size;
}