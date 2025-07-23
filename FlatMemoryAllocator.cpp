#include "FlatMemoryAllocator.h"
#include <string>
#include <memory>

FlatMemoryAllocator::FlatMemoryAllocator(uint16_t maxSize) {
	this->maxSize = maxSize;
	this->allocatedSize = 0;
	memory.resize(maxSize);
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
	if (!ptr) return; // Ignore null pointers

	char* base = &memory[0];
	char* end = base + memory.size();
	char* cptr = static_cast<char*>(ptr);

	if (cptr < base || cptr >= end) {
		// Invalid pointer, ignore or handle error
		return;
	}

	uint16_t index = static_cast<uint16_t>(cptr - base);
	// Only deallocate if this is the start of a block
	if (blockSizes.count(index) > 0) {
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
	lock_guard<mutex> lock(memMutex);
	blockSizes[index] = size; // save size for deallocation purposes
	for (uint16_t i = index; i < index + size; ++i) { // [index, index+size)
		allocationMap[i] = true;  // Mark as allocated
		memory[i] = 'X'; // Mark allocated memory with 'X'
	}
	allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(uint16_t index) {
	lock_guard<mutex> lock(memMutex);
	uint16_t size = blockSizes[index];
	for (uint16_t i = index; i < index + size; ++i) {
		allocationMap[i] = false;
		memory[i] = '.'; // Mark deallocated memory with '.'
	}
	blockSizes.erase(index);
	allocatedSize -= size;
}

char* FlatMemoryAllocator::getMemoryBase() {
	return memory.data();
}

uint16_t FlatMemoryAllocator::getBlockSizeAt(uint16_t index) const {
	auto it = blockSizes.find(index);
	if (it != blockSizes.end()) {
		return it->second;
	}
	return 0;
}

uint16_t FlatMemoryAllocator::getMaxSize() const {
	return maxSize;
}

size_t FlatMemoryAllocator::getTotalExtFrag() const {
	lock_guard<mutex> lock(memMutex);
	return count(memory.begin(), memory.end(), '.');
}
