#include "PagingAllocator.h"
#include <iostream>
#include <random>
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

// Allocates memory (in frames) for a given process.
bool PagingAllocator::Allocate(shared_ptr<Process> process) {

    //what we allocate here is the needed frame of the process
    size_t processID = process->getPID();

    size_t numFramesNeeded = process->getMemReq() / frameSize;

    

    //vector<size_t> allocated = allocateFrames(numFramesNeeded, processID);
    process->setPages(false);
    return true;
}

bool PagingAllocator::AllocatePage(shared_ptr<Process> process, uint16_t pageIndex) {
    size_t processID = process->getPID();
    size_t numFramesNeeded = 1; // Allocating one frame for the page
    if (freeFrameList.empty()) {
        if (!frameMap.empty()) {
            
            // Randomly select a frame to evict
            std::vector<size_t> frameIndices;
            for (const auto& pair : frameMap) {
                frameIndices.push_back(pair.first);
            }
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, static_cast<int>(frameIndices.size() - 1));
            size_t victimFrame = frameIndices[dis(gen)];

            // Remove the victim frame from frameMap and add to freeFrameList
            frameMap.erase(victimFrame);
            freeFrameList.push_back(victimFrame);
			// also remove the allocated frame from the process
			process->removeFromAllocatedFrames(victimFrame);
            
            std::cout << "Page Swapping: Evicted frame " << victimFrame << " from memory.\n";
            
        }
        else {
            // No frames to evict, allocation fails
            return false;
        }
    }
	// add the frame to the process's allocated frames
    size_t allocated = allocateSingleFrameForPage(process, pageIndex);
    process->addToAllocatedFrames(allocated);
	process->addToPageIndices(pageIndex);
	 // Add the frame to the process's allocated memory
    /*
        // Associate the allocated frame with the specific page
        FrameEntry& entry = frameMap[allocated[0]];
        entry.pageNumber = pageIndex;
        process->addPageToFrame(pageIndex, allocated[0]); // Add mapping of page to frame
    */
    return true;
}

// Frees the memory allocated to a given process.
void PagingAllocator::Deallocate(shared_ptr<Process> process) {
    for (size_t frame : process->getAllocatedFrames()) {
        frameMap[frame].valid = false;
        frameMap.erase(frame);
        freeFrameList.push_back(frame);
    }

    process->setPages(true); // Clear
}

// Prints the current memory status (frame-by-frame).
void PagingAllocator::visualizeMemory() const {
    cout << "Memory Visualization:\n";
	// for each frame, print its contents
    for (size_t frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
        auto it = frameMap.find(frameIndex);
        if (it != frameMap.end()) {
            const FrameEntry& entry = it->second;
            std::cout << "Frame " << frameIndex
                << " (Process " << entry.processID
                << ", Page " << entry.pageNumber << "): ";

            /*
            for (size_t i = 0; i < entry.memoryContents.size(); ++i) {
                int value = entry.memoryContents[i];

                if (value == -1) {
                    std::cout << ". ";
                }
                else {
                    // If there's a corresponding variable name, print it
                    if (i < entry.memoryContentsVarName.size() && !entry.memoryContentsVarName[i].empty()) {
                        //std::cout << entry.memoryContentsVarName[i] << "(" << value << ") ";
                        std::cout << value << " ";
                    }
                    else {
                        std::cout << value << " ";
                    }
                }
            }
            */
            std::cout << "\n";
            
        }
        else {
            std::cout << "Frame " << frameIndex << " -> Free\n";
        }
    }
    std::cout << "---------------------------\n";
}

size_t PagingAllocator::getMemoryUsed() const {
    return frameMap.size() * frameSize; // Each frame is of size frameSize
}

// Helper function to allocate specific number of frames to a process.
vector<size_t> PagingAllocator::allocateFrames(uint16_t numFrames, uint16_t processID) {
    vector<size_t> allocatedFrames;

    for (uint16_t i = 0; i < numFrames; ++i) {
        if (freeFrameList.empty()) break;

        size_t frame = freeFrameList.front();
        freeFrameList.erase(freeFrameList.begin());

        FrameEntry entry;
        entry.valid = true; // valid bit if in main memory map
        entry.processID = processID;
        entry.pageNumber = i;
        entry.memoryContents = vector<int>(frameSize, -1); // default to 0
        entry.memoryContentsVarName = vector<string>(frameSize, ""); // default empty names

        frameMap[frame] = entry;
        allocatedFrames.push_back(frame);
    }

    /*
    cout << endl;
    for (uint16_t bit : freeFrameList) {
        cout << bit << ", ";
    }
    cout << endl;
    */
    return allocatedFrames;
}

bool PagingAllocator::allocateSingleFrameForPage(shared_ptr<Process> process, uint16_t pageIndex) {
    if (freeFrameList.empty()) {
        return false; // No free frame available
    }

    size_t frame = freeFrameList.front();
    freeFrameList.erase(freeFrameList.begin());

    FrameEntry entry;
    entry.valid = true;
    entry.processID = process->getPID();
    entry.pageNumber = pageIndex;

    // Copy the process's page data into the frame's memoryContents
    // (Assuming processMemoryRead[pageIndex] holds the data for this page)
    const auto& processPages = process->getProcessMemoryRead();
    if (pageIndex < processPages.size()) {
        entry.memoryContents = processPages[pageIndex];
    }
    else {
        entry.memoryContents = vector<int>(frameSize, -1);
    }
    entry.memoryContentsVarName = vector<string>(frameSize, "");

    frameMap[frame] = entry;

    return true;
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

