#pragma once
#include <cstddef> // for size_t
#include <string>
#include <vector>
#include <unordered_map>
#include "MemoryAllocator.h"

using namespace std;

class FlatMemoryAllocator : public MemoryAllocator {
public:
	FlatMemoryAllocator(uint16_t maxSize);
	~FlatMemoryAllocator();
	void* Allocate(uint16_t size) override;
	void Deallocate(void* ptr) override;
	string visualizeMemory() const override;
	char* getMemoryBase();
	uint16_t getBlockSizeAt(uint16_t index) const;
	uint16_t getMaxSize() const;
	size_t getTotalExtFrag() const;

private:
	uint16_t maxSize;
	uint16_t allocatedSize;
	vector<char> memory;
	unordered_map<uint16_t, bool> allocationMap; // Maps start address to allocation status
	unordered_map<uint16_t, uint16_t> blockSizes; // start index -> size; for deallocation purposes
	void initializeMemory();
	bool canAllocateAt(uint16_t index, uint16_t size) const;
	void allocateAt(uint16_t index, uint16_t size);
	void deallocateAt(uint16_t index);



};