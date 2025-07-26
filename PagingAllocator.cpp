#include "PagingAllocator.h"
#include <iostream>
using namespace std;

PagingAllocator::PagingAllocator(uint16_t maxMem)
    : maxMem(maxMem), numFrames(maxMem) {
    // Initialize the free frame list
    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(i);
    }
}

void* PagingAllocator::Allocate(shared_ptr<Process> process) {
    size_t processId = process->getPID();
    size_t numFramesNeeded = (process->getMemReq()) / frameSize;
    if (numFramesNeeded > freeFrameList.size()) {
       cout << "Memory allocation failed. Not enough free frames.\n";
       return nullptr;
    }

    // Allocate frames for the process
    size_t frameIndex = allocateFrames(numFramesNeeded, processId);
    return reinterpret_cast<void*>(frameIndex);
}

void PagingAllocator::Deallocate(shared_ptr<Process> process) {
    size_t processId = process->getPID();

    // Find frames allocated to the process and deallocate
    auto it = std::find_if(frameMap.begin(), frameMap.end(),
        [processId](const auto& entry) { return entry.second == processId; });

    while (it != frameMap.end()) {
        size_t frameIndex = it->first;
        deallocateFrames(1, frameIndex);
        it = std::find_if(frameMap.begin(), frameMap.end(),
            [processId](const auto& entry) { return entry.second == processId; });
    }
}

void PagingAllocator::visualizeMemory() const {
    cout << "Memory Visualization:\n";
    for (size_t frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
        auto it = frameMap.find(frameIndex);
        if (it != frameMap.end()) {
            std::cout << "Frame " << frameIndex << " -> Process " << it->second << "\n";
        }
        else {
            std::cout << "Frame " << frameIndex << " -> Free\n";
        }
    }
    std::cout << "---------------------------\n";
}

uint16_t PagingAllocator::allocateFrames(uint16_t numFrames, uint16_t processId) {
    size_t frameIndex = freeFrameList.back();
    freeFrameList.pop_back();

    // Map allocated frames to the process ID
    for (size_t i = 0; i < numFrames; ++i) {
        frameMap[frameIndex + i] = processId;
    }

    return frameIndex;
}

void PagingAllocator::deallocateFrames(uint16_t numFrames, uint16_t frameIndex) {
    // Remove mapping of deallocated frames
    for (size_t i = 0; i < numFrames; ++i) {
        frameMap.erase(frameIndex + i);
    }

    // Add frames back to the free frame list
    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(frameIndex + i);
    }
}
