#pragma once
#include <queue>
#include <memory>
#include "Process.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "IMemoryAllocator.h"


using namespace std;

class Scheduler {
public:
    enum class Mode {
        FCFS,
        RR
    };

    Scheduler(Mode mode, uint32_t timeQuantum = 1, int coreCount = 1, uint32_t delay = 1, shared_ptr<IMemoryAllocator> memoryAllocator = nullptr, size_t minins = 0, size_t maxins = 0);
    void addProcess(shared_ptr<Process> process);
    void run();
    vector<shared_ptr<Process>> getRunningQueue();
    vector<shared_ptr<Process>> getFinishedQueue();
    int getRunningCores();
    int getFreeCores();
    void writeMemorySnapshot(int quantumCycle);

private:
    void coreWorker(int coreID); // Worker function per core
    Mode schedulingMode;
    uint32_t timeQuantum;
    uint32_t cpuCoreCount;
    uint32_t delay;

    shared_ptr<IMemoryAllocator> memoryAllocator;
    queue<shared_ptr<Process>> readyQueue;
    mutex queueMutex;
    condition_variable queueCV;
    size_t minins;
	size_t maxins;

    vector<shared_ptr<Process>> runningQueue;
    vector<shared_ptr<Process>> finishedQueue;
    vector<shared_ptr<Process>> sleepQueue;
    vector<thread> cores;
	//vector<atomic<bool>> coreBusy; // Track if a core is busy
    void tickSleepers();
};