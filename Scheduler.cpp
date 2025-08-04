#pragma once
#include "Scheduler.h"
#include "IMemoryAllocator.h"
#include <iostream>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>


mutex coutMutex;

Scheduler::Scheduler(Mode mode, uint32_t quantum, int coreCount, uint32_t delay, shared_ptr<IMemoryAllocator> memoryAllocator, size_t minins, size_t maxins)
    : schedulingMode(mode), timeQuantum(quantum), cpuCoreCount(coreCount), delay(delay), memoryAllocator(memoryAllocator) {
    //for (auto& busy : coreBusy) busy = false;
	this->minins = minins;
	this->maxins = maxins;
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

queue<shared_ptr<Process>> Scheduler::getReadyQueue() {
    return readyQueue;
}

void Scheduler::writeMemorySnapshot(int quantumCycle) {
    lock_guard<mutex> lock(queueMutex);
    ostringstream filename;
    filename << "memory_stamp_" << setw(2) << setfill('0') << quantumCycle << ".txt";
    ofstream out(filename.str());

    // Generate timestamp
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm* now_tm = localtime(&now_c);
    out << "Timestamp: (" << put_time(now_tm, "%m/%d/%Y %I:%M:%S%p") << ")\n";

    vector<tuple<uint16_t, string, uint16_t>> memoryBlocks;
    int processCount = 0;

    vector<shared_ptr<Process>> allProcesses;

    // From runningQueue (vector)
    for (const auto& process : runningQueue) {
        if (process) allProcesses.push_back(process);
    }

    // From sleepQueue (vector)
    for (const auto& process : sleepQueue) {
        if (process) allProcesses.push_back(process);
    }

    // From readyQueue (queue)
    queue<shared_ptr<Process>> tempQueue = readyQueue; // copy so we don't modify the original
    while (!tempQueue.empty()) {
        auto process = tempQueue.front();
        tempQueue.pop();
        if (process) allProcesses.push_back(process);
    }

    for (const auto& process : allProcesses) {
        if (process != nullptr) {
            void* ptr = process->getAllocatedMemory();
            if (ptr != nullptr) {
                char* base = memoryAllocator->getMemoryBase();
                char* cptr = static_cast<char*>(ptr);
                uint16_t lower = static_cast<uint16_t>(cptr - base);
                uint16_t size = memoryAllocator->getBlockSizeAt(lower);
                uint16_t upper = lower + size;
                memoryBlocks.emplace_back(upper, process->getName(), lower);
                ++processCount;
            }
        }
    }

    out << "Number of processes in memory: " << processCount << "\n";

    uint16_t fragmentationBytes = memoryAllocator->getTotalExtFrag();
    out << "Total external fragmentation in KB: " << fragmentationBytes << "\n\n";

    out << "----end---- = " << memoryAllocator->getMaxSize() << "\n\n";

    sort(memoryBlocks.rbegin(), memoryBlocks.rend()); // Sort by upper in descending order

    for (const auto& block : memoryBlocks) {
        uint16_t upper, lower;
        string name;
        tie(upper, name, lower) = block;

        out << upper << "\n" << name << "\n" << lower << "\n\n";
    }

    out << "----start---- = 0\n";
    out.close();
}




void Scheduler::coreWorker(int coreID) {
    bool allocatedFrames = false;
    while (true) {
        shared_ptr<Process> current = nullptr;
        
        
        {
            unique_lock<mutex> lock(queueMutex);
            queueCV.wait(lock, [this] { return !readyQueue.empty(); }); // Wait until queue is not empty
            current = readyQueue.front();
            //cout << "Process " << current->getName() << " now in running queue\n";
            if (current->getAllocatedFrames().empty()) {
                allocatedFrames = memoryAllocator->Allocate(current);
                if (!allocatedFrames) {
                    //std::cout << "Insufficient memory for process " << current->getName() << " (ID: " << current->getPID() << ")\n";
                    readyQueue.pop();
                    readyQueue.push(current);
                    continue; // Skip to next iteration, don't run this process
                }
                //cout << "Process " << current->getName() << " (ID: " << current->getPID() << ") allocated memory at address: " << allocatedMemory << "\n";
                //std::cout << "Allocated memory for process " << current->getName() << " (ID: " << current->getPID() << ")\n";
                //memoryAllocator->visualizeMemory();
            }

            if(current->getHasCommands() == false) {
                current->generateCommands(minins, maxins, 0);
				current->setHasCommands(true);
		    }

            // If already has memory, just proceed
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
            while (!current->isFinished() && !current->getIsTerminated()) {

                // add to running queue
                activeTicks++;
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
                if (!current->getAllocatedFrames().empty()) {
                    memoryAllocator->Deallocate(current);
                    //std::cout << "Deallocated memory for process " << current->getName() << " (ID: " << current->getPID() << ")\n";
                    //std::cout << "Memory state: " << memoryAllocator->visualizeMemory() << "\n";
                }
				
                //current->writeLogsToFile(current->getName() + "_logs.txt");
            }
        }

        else if (schedulingMode == Mode::RR) {
            current->setTimestep(0);
            bool didSleep = false;
            bool justFinished = false;

            // until finished or timeQuantum is reached
            while (!current->isFinished() && current -> getTimestep() < timeQuantum) {

                activeTicks++;
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


                //writeMemorySnapshot(timestep);

                current->setTimestep(current->getTimestep() + 1);
                //cout << current->getTimestep();

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
                    if (!current->getAllocatedFrames().empty()) {
                        memoryAllocator->Deallocate(current);
                        //std::cout << "Deallocated memory for process " << current->getName() << " (ID: " << current->getPID() << ")\n";
                        //memoryAllocator->visualizeMemory();
                    }
                    justFinished = true;  // Mark for later
                }
            }

           
        }

        //coreBusy[coreID] = false; // core now free
        
    }
}