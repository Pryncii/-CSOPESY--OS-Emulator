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

    thread sleeperThread(&Scheduler::tickSleepers, this);
    sleeperThread.detach();

    // wait until all cores finish
    for (auto& t : cores) {
		//coreBusy[t.get_id()] = false; // mark core as free
        t.detach();
    }
}

void Scheduler::tickSleepers() {
    
    while (true) {
        {
            lock_guard<mutex> lock(queueMutex);
            
            auto it = sleepQueue.begin();
            while (it != sleepQueue.end()) {
                auto& proc = *it;
                proc->decrementSleepTick();
                //cout << proc->getName() << "Honk shoo mimimimi\n";
                if (proc->getSleepTicks() <= 0) {
                    proc->setSleeping(false);
                    //cout << proc->getName() << "Woke up\n";
                    readyQueue.push(proc);
                    queueCV.notify_one();
                    it = sleepQueue.erase(it);  // remove from sleep queue
                }
                else {
                    ++it;
                }
            }
        }
        //this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

int Scheduler::getRunningCores() {
    return getRunningQueue().size();
}


vector<shared_ptr<Process>> Scheduler::getRunningQueue() {
    return runningQueue;
}

vector<shared_ptr<Process>> Scheduler::getFinishedQueue() {
    return finishedQueue;
}

void Scheduler::coreWorker(int coreID) {
    while (true) {
        shared_ptr<Process> current = nullptr;

        {
            unique_lock<mutex> lock(queueMutex);
            queueCV.wait(lock, [this] { return !readyQueue.empty(); }); // Wait until queue is not empty
            current = readyQueue.front();
            //cout << "Process " << current->getName() << " now in running queue\n";

            readyQueue.pop();
            runningQueue.push_back(current);
            current->setCpuCoreID(coreID);
        }

        //coreBusy[coreID] = true; // core is now busy

        
        {
            //lock_guard<mutex> lock(coutMutex);
            //cout << "\n[Core " << coreID << "] Running PID: " << current->getPID() << "\n";
        }


        if (schedulingMode == Mode::FCFS) {
            // run until finished
            bool didSleep = false;
            while (!current->isFinished()) {

                // add to running queue
                current->executeCommand();
                current->moveToNextLine();

                if (current->isSleeping()) {
                    lock_guard<mutex> lock(queueMutex);

                    // Remove from runningQueue
                    for (size_t i = 0; i < runningQueue.size(); ++i) {
                        if (runningQueue[i]->getPID() == current->getPID()) {
                            runningQueue.erase(runningQueue.begin() + i);
                            break;
                        }
                    }
                    sleepQueue.push_back(current);  // Move to sleepQueue
                    didSleep = true;
                    //cout << current->getName() << " is sleeping!\n";
                    break;  // Stop executing this process
                }
                this_thread::sleep_for(chrono::milliseconds(delay));
            }
            
            if (!didSleep) {
                lock_guard<mutex> lock(queueMutex);
                
                //cout << "\nProcess PID " << current->getPID() << " finished.\n";

                for (size_t i = 0; i < runningQueue.size(); ++i) {
                    if (runningQueue[i]->getPID() == current->getPID()) {
                        runningQueue.erase(runningQueue.begin() + i);  // remove the process
                        break; // break when found and removed
                    }
                }
                finishedQueue.push_back(current); // add to finished queue
				
                //current->writeLogsToFile(current->getName() + "_logs.txt");
            }
        }

        else if (schedulingMode == Mode::RR) {
            int timestep = 0;
            bool didSleep = false;
            // until finished or timeQuantum is reached
            while (!current->isFinished() && timestep < timeQuantum) {
                current->executeCommand();
                current->moveToNextLine();

                if (current->isSleeping()) {
                    lock_guard<mutex> lock(queueMutex);

                    // Remove from runningQueue
                    for (size_t i = 0; i < runningQueue.size(); ++i) {
                        if (runningQueue[i]->getPID() == current->getPID()) {
                            runningQueue.erase(runningQueue.begin() + i);
                            break;
                        }
                    }
                    sleepQueue.push_back(current);  // Move to sleepQueue
                    didSleep = true;
                    //cout << current->getCpuCoreID() << current->getName() << " is sleeping!\n";
                    break;  // Stop executing this process
                }
                    
                timestep++;
                //cout << current->getCpuCoreID() << "'s Time step: " << timestep << "\n";
                //if (timestep == timeQuantum) {
                //    cout << current->getName() << " kicked out of " << current->getCpuCoreID() << "!\n";
                //}
                this_thread::sleep_for(chrono::milliseconds(delay));
            }

            if (!didSleep) {
                lock_guard<mutex> lock(queueMutex);
                // remove the current from runningQueue if not done running and time quantum reached
                for (size_t i = 0; i < runningQueue.size(); ++i) {
                    if (runningQueue[i]->getPID() == current->getPID()) {
                        runningQueue.erase(runningQueue.begin() + i);  // remove the process
                        break; // break when found and removed
                    }
                }
                if (!current->isFinished()) {
                    //cout << "\nTime slice ended for Process PID " << current->getPID() << current->getCurLine() << "\n";
                    readyQueue.push(current); // not done, add back to queue
                }
                else {
                    //cout << "\nProcess PID " << current->getPID() << " finished.\n";
                    finishedQueue.push_back(current); // add to finished queue
                    //current->writeLogsToFile(current->getName() + "_logs.txt");
                }
            }    
        }

        //coreBusy[coreID] = false; // core now free
        
    }
}