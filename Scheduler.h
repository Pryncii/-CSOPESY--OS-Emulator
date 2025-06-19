#pragma once
#include <queue>
#include <memory>
#include "Process.h"
#include <thread>
#include <mutex>

using namespace std;

class Scheduler {
public:
    enum class Mode {
        FCFS,
        RR
    };

    Scheduler(Mode mode, int timeQuantum = 1, int coreCount = 1);
    void addProcess(shared_ptr<Process> process);
    void run();

private:
    void coreWorker(int coreID); // Worker function per core
    Mode schedulingMode;
    int timeQuantum;
    int cpuCoreCount;

    queue<shared_ptr<Process>> readyQueue;
    mutex queueMutex;

    vector<thread> cores;
};
