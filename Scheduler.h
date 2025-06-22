#pragma once
#include <queue>
#include <memory>
#include "Process.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>


using namespace std;

class Scheduler {
public:
    enum class Mode {
        FCFS,
        RR
    };

    Scheduler(Mode mode, uint32_t timeQuantum = 1, int coreCount = 1, uint32_t delay = 1);
    void addProcess(shared_ptr<Process> process);
    void run();
    queue<shared_ptr<Process>> getRunningQueue();
    queue<shared_ptr<Process>> getFinishedQueue();
    int getRunningCores();
    int getFreeCores();

private:
    void coreWorker(int coreID); // Worker function per core
    Mode schedulingMode;
    uint32_t timeQuantum;
    uint32_t cpuCoreCount;
    uint32_t delay;

    queue<shared_ptr<Process>> readyQueue;
    mutex queueMutex;
    condition_variable queueCV;

    queue<shared_ptr<Process>> runningQueue;
    queue<shared_ptr<Process>> finishedQueue;
    vector<thread> cores;
	//vector<atomic<bool>> coreBusy; // Track if a core is busy
};