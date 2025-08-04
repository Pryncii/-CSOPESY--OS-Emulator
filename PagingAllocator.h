#pragma once
#include "IMemoryAllocator.h"
#include <unordered_map>
#include <vector>

#include "FrameEntry.h"

class PagingAllocator : public IMemoryAllocator
{
public:
	PagingAllocator(uint16_t maxMem, uint16_t memFrame);
	bool Allocate(shared_ptr<Process> process) override;
	void Deallocate(shared_ptr<Process> process) override;
	void visualizeMemory() const override;
	char* getMemoryBase() override;
	uint16_t getBlockSizeAt(uint16_t index) const override;
	size_t getTotalExtFrag() const override;
	uint16_t getMaxSize() const override;
	//no override here
	unordered_map<uint16_t, FrameEntry> getFrameMap() const override { return frameMap; }
	size_t getMemoryUsed() const override;

private:
	uint16_t maxMem;
	uint16_t numFrames;
	uint16_t frameSize;
	unordered_map<uint16_t, FrameEntry> frameMap; // key = phys frame index, value {process-id, page number}
	vector<uint16_t> freeFrameList; 

	vector<size_t> PagingAllocator::allocateFrames(uint16_t numFrames, uint16_t processID);
	//void deallocateFrames(uint16_t numFrames, uint16_t frameIndex);
};

