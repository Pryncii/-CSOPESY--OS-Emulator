#include "FlatMemoryAllocator.h"
#include <string>
#include <memory>
#include <iostream>

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
    // 1. Validate input
    if (size == 0 || size > maxSize) return nullptr;

    // 2. Check initialization
    if (allocationMap.empty() || memory.empty()) {
        return nullptr;
    }

    // 3. Safe allocation search
    const uint16_t endPos = maxSize - size + 1;
    for (uint16_t i = 0; i < endPos; ) {
        // Use find() instead of direct access
        auto it = allocationMap.find(i);
        

        if (!it->second) { // Found free block
            bool contiguousFree = true;

            // Check if we have enough contiguous space
            for (uint16_t j = 1; j < size; ++j) {
                auto nextIt = allocationMap.find(i + j);
                if (nextIt == allocationMap.end() || nextIt->second) {
                    contiguousFree = false;
                    i += j; // Skip ahead
                    break;
                }
            }

            if (contiguousFree) {
                // Mark all blocks as allocated
                for (uint16_t j = 0; j < size; ++j) {
                    allocationMap[i + j] = true;
                }
                return &memory[i];
            }
        }
        else {
            i++;
        }
    }
    return nullptr;
}
void FlatMemoryAllocator::Deallocate(void* ptr) {
	uint16_t index = static_cast<char*>(ptr) - &memory[0];
	if(allocationMap[index]) {
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
	/*for (uint16_t i = 0; i < maxSize; ++i) {
		allocationMap[i] = false; // mark each byte index as free
	}*/
	// Pre-populate all possible keys with false
	for (uint16_t i = 0; i < maxSize; ++i) {
		allocationMap.insert({ i, false }); // Explicit insertion
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