#pragma once
#include "Scheduler.h"
#include <iostream>
#include <mutex>

mutex coutMutex;

Scheduler::Scheduler(Mode mode, int quantum, int coreCount)
    : schedulingMode(mode), timeQuantum(quantum), cpuCoreCount(coreCount) {
}

void Scheduler::addProcess(shared_ptr<Process> process) {
    lock_guard<mutex> lock(queueMutex);
    readyQueue.push(process);
}

void Scheduler::run() {
    for (int i = 0; i < cpuCoreCount; ++i) {
        cores.emplace_back(&Scheduler::coreWorker, this, i); // create and start a new thread running coreWorker(i) on core i
    }

    // wait until all cores finish
    for (auto& t : cores) {
        t.detach();
    }
}

void Scheduler::coreWorker(int coreID) {
    while (true) {
        shared_ptr<Process> current = nullptr;

        {
            lock_guard<mutex> lock(queueMutex);
            if (readyQueue.empty()) return;
            current = readyQueue.front();
            readyQueue.pop();
        }

        current->setCpuCoreID(coreID);
        {
            //lock_guard<mutex> lock(coutMutex);
            cout << "[Core " << coreID << "] Running PID: " << current->getPid() << "\n";
        }
        

        if (schedulingMode == Mode::FCFS) {
            // run until finished
            while (!current->isFinished()) {
                current->executeCommand();
                current->moveToNextLine();
            }
            {
                //lock_guard<mutex> lock(coutMutex);
                cout << "Process PID " << current->getPid() << " finished.\n";
            }

        }
        else if (schedulingMode == Mode::RR) {
            int timestep = 0;
            // until finished or timeQuantum is reached
            while (!current->isFinished() && timestep < timeQuantum) {
                current->executeCommand();
                current->moveToNextLine();
                timestep++;
            }

            if (!current->isFinished()) {
                lock_guard<mutex> lock(queueMutex);
                readyQueue.push(current); // not done, add back to queue
            }
            else {
                //lock_guard<mutex> lock(coutMutex);
                cout << "Process PID " << current->getPid() << " finished.\n";
            }
        }
    }
}
