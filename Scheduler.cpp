#pragma once
#include "Scheduler.h"
#include <iostream>
#include <mutex>

mutex coutMutex;

Scheduler::Scheduler(Mode mode, uint32_t quantum, int coreCount, uint32_t delay)
    : schedulingMode(mode), timeQuantum(quantum), cpuCoreCount(coreCount), delay(delay) {
    //for (auto& busy : coreBusy) busy = false;
}

void Scheduler::addProcess(shared_ptr<Process> process) {
    {
        lock_guard<mutex> lock(queueMutex);
        readyQueue.push(process);
    }
    queueCV.notify_one(); // Notify one waiting worker
}

void Scheduler::run() {
    for (int i = 0; i < cpuCoreCount; ++i) {
        cores.emplace_back(&Scheduler::coreWorker, this, i); // create and start a new thread running coreWorker(i) on core i
    }

    // wait until all cores finish
    for (auto& t : cores) {
		//coreBusy[t.get_id()] = false; // mark core as free
        t.detach();
    }
}

int Scheduler::getRunningCores() {
    return getRunningQueue().size();
}


queue<shared_ptr<Process>> Scheduler::getRunningQueue() {
    return runningQueue;
}

queue<shared_ptr<Process>> Scheduler::getFinishedQueue() {
    return finishedQueue;
}

void Scheduler::coreWorker(int coreID) {
    while (true) {
        shared_ptr<Process> current = nullptr;

        {
            unique_lock<mutex> lock(queueMutex);
            queueCV.wait(lock, [this] { return !readyQueue.empty(); }); // Wait until queue is not empty
            current = readyQueue.front();
            readyQueue.pop();
            runningQueue.push(current);
        }

        //coreBusy[coreID] = true; // core is now busy

        current->setCpuCoreID(coreID);
        {
            //lock_guard<mutex> lock(coutMutex);
           // cout << "[Core " << coreID << "] Running PID: " << current->getPID() << "\n";
        }


        if (schedulingMode == Mode::FCFS) {
            // run until finished
            while (!current->isFinished()) {


				
                // add to running queue
                current->executeCommand();
                current->moveToNextLine();
                this_thread::sleep_for(chrono::milliseconds(delay));
            }
            
            {
                //lock_guard<mutex> lock(coutMutex);
                //cout << "Process PID " << current->getPID() << " finished.\n";
				finishedQueue.push(current); // add to finished queue
				runningQueue.pop(); // remove from running queue
                //current->writeLogsToFile(current->getName() + "_logs.txt");
            }

        }
        else if (schedulingMode == Mode::RR) {
            int timestep = 0;
            // until finished or timeQuantum is reached
            while (!current->isFinished() && timestep < timeQuantum) {
                current->executeCommand();
                current->moveToNextLine();
                timestep++;
                this_thread::sleep_for(chrono::milliseconds(delay));
            }

            if (!current->isFinished()) {
                lock_guard<mutex> lock(queueMutex);
                readyQueue.push(current); // not done, add back to queue
            }
            else {
                //lock_guard<mutex> lock(coutMutex);
                //cout << "Process PID " << current->getPID() << " finished.\n";
                //cout << "Process PID " << current->getPID() << " finished.\n";

                finishedQueue.push(current); // add to finished queue
                runningQueue.pop(); // remove from running queue
            }
        }

        //coreBusy[coreID] = false; // core now free
        
    }
}