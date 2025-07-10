#pragma once
#include <queue>
#include <memory>
#include "Process.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "MemoryAllocator.h"


using namespace std;

class Scheduler {
public:
    enum class Mode {
        FCFS,
        RR
    };

    Scheduler(Mode mode, uint32_t timeQuantum, int coreCount, uint32_t delay, MemoryAllocator& memoryAllocator);
    void addProcess(shared_ptr<Process> process);
    void run();
    vector<shared_ptr<Process>> getRunningQueue();
    vector<shared_ptr<Process>> getFinishedQueue();
    int getRunningCores();
    int getFreeCores();

private:
    void coreWorker(int coreID); // Worker function per core
    Mode schedulingMode;
    uint32_t timeQuantum;
    uint32_t cpuCoreCount;
    uint32_t delay;
    MemoryAllocator& memoryAllocator;

    queue<shared_ptr<Process>> readyQueue;
    mutex queueMutex;
    condition_variable queueCV;

    vector<shared_ptr<Process>> runningQueue;
    vector<shared_ptr<Process>> finishedQueue;
    vector<shared_ptr<Process>> sleepQueue;
    vector<thread> cores;
	//vector<atomic<bool>> coreBusy; // Track if a core is busy
    void tickSleepers();
};