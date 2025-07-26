#pragma once
#include "IMemoryAllocator.h"
#include <unordered_map>
#include <vector>
class PagingAllocator : public IMemoryAllocator
{
public:
	PagingAllocator(uint16_t maxMem);
	void* Allocate(shared_ptr<Process> process) override;
	void Deallocate(shared_ptr<Process> process) override;
	void visualizeMemory() const override;

private:
	uint16_t maxMem;
	uint16_t numFrames;
	uint16_t frameSize;
	unordered_map<uint16_t, uint16_t> frameMap; 
	vector<uint16_t> freeFrameList; 

	uint16_t allocateFrames(uint16_t numFrames, uint16_t PID);
	void deallocateFrames(uint16_t numFrames, uint16_t frameIndex);
	
};

