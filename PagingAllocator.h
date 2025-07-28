#pragma once
#include "IMemoryAllocator.h"
#include <unordered_map>
#include <vector>
class PagingAllocator : public IMemoryAllocator
{
public:
	PagingAllocator(uint16_t maxMem, uint16_t memFrame);
	void* Allocate(shared_ptr<Process> process) override;
	void Deallocate(shared_ptr<Process> process) override;
	void visualizeMemory() const override;
	char* getMemoryBase() override;
	uint16_t getBlockSizeAt(uint16_t index) const override;
	size_t getTotalExtFrag() const override;
	uint16_t getMaxSize() const override;

private:
	uint16_t maxMem;
	uint16_t numFrames;
	uint16_t frameSize;
	unordered_map<uint16_t, uint16_t> frameMap; 
	vector<uint16_t> freeFrameList; 

	vector<size_t> PagingAllocator::allocateFrames(uint16_t numFrames, uint16_t processID);
	//void deallocateFrames(uint16_t numFrames, uint16_t frameIndex);
};

