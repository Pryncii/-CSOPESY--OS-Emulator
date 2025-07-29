#include "PagingAllocator.h"
#include <iostream>
using namespace std;

PagingAllocator::PagingAllocator(uint16_t maxMem, uint16_t memFrame)
    : maxMem(maxMem), numFrames(maxMem/memFrame), frameSize(memFrame) {
    // Initialize the free frame list
    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(i);
        cout << freeFrameList[i] << ", ";
    }
    cout << endl;

    
}

void* PagingAllocator::Allocate(shared_ptr<Process> process) {
    size_t processID = process->getPID();
    size_t numFramesNeeded = (process->getMemReq()) / frameSize;

    if (numFramesNeeded > freeFrameList.size()) {
        cout << "Memory allocation failed. Not enough free frames.\n";
        return nullptr;
    }

    vector<size_t> allocated = allocateFrames(numFramesNeeded, processID);
    process->setAllocatedFrames(allocated); // Store in the process

    return reinterpret_cast<void*>(1); // dummy non-null value to show success
}

void PagingAllocator::Deallocate(shared_ptr<Process> process) {
    for (size_t frame : process->getAllocatedFrames()) {
        frameMap.erase(frame);
        freeFrameList.push_back(frame);
    }

    process->setAllocatedFrames({}); // Clear
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

vector<size_t> PagingAllocator::allocateFrames(uint16_t numFrames, uint16_t processID) {
    vector<size_t> allocatedFrames;

    for (uint16_t i = 0; i < numFrames; ++i) {
        if (freeFrameList.empty()) break;

        size_t frame = freeFrameList.front();
        freeFrameList.erase(freeFrameList.begin());

        frameMap[frame] = processID;
        allocatedFrames.push_back(frame);
    }

    cout << endl;
    for (uint16_t bit : freeFrameList) {
        cout << bit << ", ";
    }
    cout << endl;
    return allocatedFrames;
}

//void PagingAllocator::deallocateFrames(uint16_t numFrames, uint16_t frameIndex) {
//    // Remove mapping of deallocated frames
//    for (size_t i = 0; i < numFrames; ++i) {
//        frameMap.erase(frameIndex + i);
//    }
//
//    // Add frames back to the free frame list
//    for (size_t i = 0; i < numFrames; ++i) {
//        freeFrameList.push_back(frameIndex + i);
//    }
//}

char* PagingAllocator::getMemoryBase() {
    return nullptr;
}

uint16_t PagingAllocator::getBlockSizeAt(uint16_t index) const {
    return 0;
}

size_t PagingAllocator::getTotalExtFrag() const {
    return 0;
}

uint16_t PagingAllocator::getMaxSize() const {
    return 0;
}

