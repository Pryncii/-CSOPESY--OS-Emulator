#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <ctime>
#include "Process.h"
#include "Command.h"
#include "PrintCommand.h"
#include "DeclareCommand.h"
#include "AddCommand.h"
#include "SubtractCommand.h"
#include "SleepCommand.h"
#include "ForLoopCommand.h"
#include "WriteCommand.h"
#include <fstream>
#include <mutex>
#include <algorithm>
#include "ReadCommand.h"

using namespace std;

Process::Process(int pid, string name, uint32_t delay, uint16_t memoryRequired, uint16_t memFrame, uint16_t maxMem) {
    time_t now = time(nullptr);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", localtime(&now));
    this->timestamp = buffer;
	this->pid = pid;
	this->name = name;
	this->commandCounter = 0;
	this->currentState = ProcessState::READY;
    this->delay = delay;
    this->memoryRequired = memoryRequired;
	this->memFrame = memFrame;
    this->maxMem = maxMem;
    
}

void Process::addCommand(shared_ptr<Command> command) {
	commandList.push_back(command);
}

int Process::getPID() const {
	return pid; 
}

int Process::getCurLine() const {
	return this->commandCounter; 
}

void Process::writeToMemory(uint16_t frameIndex, uint16_t address, uint16_t value) {
    uint16_t indexInFrame = address - (frameIndex * memFrame);
	processMemory[frameIndex][indexInFrame] = true; // Mark the memory as used
	processMemoryRead[frameIndex][indexInFrame] = value; // Store the value in the read memory
    visualizeProcessMemory();
}

void Process::readMemory(uint16_t frameIndex, uint16_t address, const string& varName) {
    uint16_t indexInFrame = address - (frameIndex * memFrame);
    int readVariable = processMemoryRead[frameIndex][indexInFrame];
    if (readVariable == -1) {
        cout << "Memory at address " << address << " in frame " << frameIndex 
             << " is not allocated or has no value." << endl;
    } else {
		allocateVariable(varName, readVariable); // Allocate the variable with the read value
        cout << "Read variable '" << varName << "' with value " << readVariable 
			<< " from address " << address << " in frame " << frameIndex << "." << endl;
	}

}

void Process::allocateVariable(const string& varName, uint16_t value) {
    for (size_t frameIdx : allocatedFrames) {
        auto& frame = processMemory[frameIdx];
		auto& frameRead = processMemoryRead[frameIdx];
        // Search for two consecutive free spots
        for (size_t i = 0; i + 1 < frame.size(); ++i) {
            if (!frame[i] && !frame[i + 1]) {
                // Allocate the two spots
                frame[i] = true;
                frame[i + 1] = true;
				frameRead[i] = value; // Store the value in the first spot
				frameRead[i + 1] = value; // Mark the second spot as unused

                // Save variable info in symbolTable only
                symbolTable[varName] = value;

                return; // Allocation successful
            }
        }
    }

    cout << "Failed to allocate variable '" << varName << "' with value " << value 
		<< ". Not enough memory available." << endl;
}

void Process::setAllocatedFrames(const vector<size_t>& frames) { 
    this->allocatedFrames = frames;
    
    cout << "For Process " << this->name << ":\n";
    for (const auto& frame : frames) {
        processMemory[frame] = vector<bool>(memFrame, false);
        cout << "Frame " << frame << ": ";
		processMemoryRead[frame] = vector<int>(memFrame, -1);
        for (bool bit : processMemory[frame]) {
            cout << bit;
        }
        cout << endl;
    }

   
}




//void Process::setAllocatedMemory(void* memory) {
//    this->allocatedMemory = memory;
//}
//
//void *Process::getAllocatedMemory() const {
//    return this->allocatedMemory;
//}

uint16_t Process::getMemReq() const {
    return this->memoryRequired;
}

void Process::visualizeProcessMemory() const {
    cout << "Process Memory Visualization for Process " << name << " (PID: " << pid << "):" << endl;
    for (size_t frameIdx : allocatedFrames) {
        cout << "Frame " << frameIdx << ": ";
        for (bool bit : processMemory.at(frameIdx)) {
            cout << (bit ? '1' : '0');
        }
        cout << endl;

        for (int bit : processMemoryRead.at(frameIdx)) {
            cout << bit << ", ";
        }
        cout << endl;
    }
}

vector<shared_ptr<Command>> Process::getCommandList() const {
    return this->commandList;
}

string Process::getTime() const {
    return timestamp;
}

int Process::getTotalLines() const {
	return countNonForInstructions(getCommandList());
}

string Process::getName() const {
	return name;
}

void Process::setCpuCoreID(int cpuCoreID) {
	this->cpuCoreID = cpuCoreID;
}

void Process::addSymbol(const string& symbol, uint16_t value) {
	symbolTable[symbol] = value;
}

unordered_map<string, uint16_t> Process::getSymbolTable() const {
	return symbolTable;
}

uint16_t Process::getSymbolValue(const string& symbol) {
	if (symbolTable.find(symbol) == symbolTable.end()) {
		addSymbol(symbol, 0); // If symbol not found, add it with value 0
	}
    
	return symbolTable.at(symbol);
}

int Process :: getCpuCoreID() const {
    return cpuCoreID;
}

void Process::executeCommand() {
	if (commandCounter < commandList.size()) {
		commandList[commandCounter]->execute();
	}
}

void Process::moveToNextLine() {
    if (commandCounter < getTotalLines()) {
        ++commandCounter;
    }
}

bool Process::isFinished() const {
	return commandCounter >= getTotalLines();
}

void Process::addLogLine(const string& logLine) {
    logLines.push_back(logLine);
}

void Process::printLogs() const {
    cout << endl;
    cout << "Process Name: " << name << endl;
	cout << "ID: " << pid << endl;
    cout << "Logs:" << endl;
    for (const auto& log : logLines) {
        cout << log << endl;
    }
            
    if (commandCounter >= getTotalLines()){
        cout << "Finished!" << endl;
    }
}

string Process::saveLogs() {

    string logtemp = "";
    for (const auto& log : logLines) {
        logtemp = logtemp + log + "\n";
    }

    string temp = "\nProcess Name: " + name + "\n"
        + "ID: " + to_string(pid) + "\n"
        + "Logs:\n" + logtemp;

    return temp;
}

//void Process::writeLogsToFile(const string& filename) const {
//    ofstream outFile(filename);
//    if (!outFile) {
//        cerr << "Failed to open file: " << filename << endl;
//        return;
//    }
//    outFile << "Process Name: " << name << endl;
//    outFile << "Logs:" << endl;
//    for (const auto& cmd : commandList) {
//        // Only log PrintCommand messages
//        auto printCmd = dynamic_pointer_cast<PrintCommand>(cmd);
//        if (printCmd) {
//            outFile << printCmd->getMessage() << endl;
//        }
//    }
//    outFile.close();
//}
	
vector<shared_ptr<Command>> Process::generateRandomCommandList(int depth, int repeats, int& instructionBudget) {
    vector<shared_ptr<Command>> commands;

    if (depth >= 3) {
        return commands;
    }

    static bool seeded = false;
    if (!seeded) {
        srand(time(0));  // Seed only once
        seeded = true;
    }

    while (instructionBudget >= repeats) {
        Command::CommandType type = static_cast<Command::CommandType>(rand() % 8); // 0 to 5
        shared_ptr<Command> cmd;
       
        switch (type) {
            case Command::DECLARE: {
                // Seeds the random number generator with the current time (in seconds).
                //srand(time(0)); // random number from 0-999 for extra unique
                string varName = to_string(pid) + "x" + to_string(depth) + "_" + to_string(rand() % 1000);
                uint16_t value = rand() % 100;
                cmd = make_shared<DeclareCommand>(shared_from_this(), varName, value);
                instructionBudget -= repeats;
				//visualizeProcessMemory();
                break;
            }

            case Command::ADD:
            case Command::SUBTRACT: {
                while (symbolTable.size() < 2) {
                    string fillerName = "autoVar_" + to_string(symbolTable.size());
                    addSymbol(fillerName, 0);
                }

                auto it1 = symbolTable.begin();
                advance(it1, rand() % symbolTable.size());
                auto it2 = symbolTable.begin();
                advance(it2, rand() % symbolTable.size());

                string varName = "x" + to_string(rand() % 1000);
                uint16_t val1 = it1->second;
                uint16_t val2 = it2->second;

                if (type == Command::ADD)
                    cmd = make_shared<AddCommand>(shared_from_this(), varName, val1, val2);
                else
                    cmd = make_shared<SubtractCommand>(shared_from_this(), varName, val1, val2);
                instructionBudget -= repeats;
                break;
            }

            case Command::PRINT: {
                cmd = make_shared<PrintCommand>(shared_from_this(), " Hello World from: ");
                instructionBudget -= repeats;
                break;
            }

            case Command::SLEEP: {
                if (depth > 1) {
                    cmd = make_shared<PrintCommand>(shared_from_this(), " Hello World from: ");
                }
                else {
                    uint16_t value = rand() % 100;
                    cmd = make_shared<SleepCommand>(shared_from_this(), value);
                }
                instructionBudget -= repeats;
                break;
            }

            case Command::FOR: {
                int looprepeats = 1 + rand() % 4;
                auto nestedCommands = generateRandomCommandList(depth + 1, looprepeats*repeats, instructionBudget);
                
                if (!nestedCommands.empty()) {
                    cmd = make_shared<ForLoopCommand>(shared_from_this(), nestedCommands, looprepeats, delay);
                }
                break;
            }

                             
            case Command::WRITE: {
                uint16_t address = 64;// +(rand() % (128 - 64));
                uint16_t value = rand() % 256;          // Random value in 0–255

                cmd = make_shared<WriteCommand>(shared_from_this(), address, memFrame, value);
                instructionBudget -= repeats;
                break;
            }


            case Command::READ: {
                uint16_t address = 64;//+(rand() % (128 - 64)); // Random address between 64–127
                string varName = "x" + to_string(rand() % 999); // e.g., x23, x75

                cmd = make_shared<ReadCommand>(shared_from_this(), address, varName, memFrame);
                instructionBudget -= repeats;
                break;
            }


            
        }
       
        if (cmd) {
            commands.push_back(cmd);
        }
	}

    
    return commands;
	//writeLogsToFile(name + "_logs.txt"); // Write logs to file after generating commands
}

void Process::generateCommands(uint32_t minIns, uint32_t maxIns, int depth) {

    if (this->totalNumCommands == -1) {
        int range = static_cast<int>(maxIns - minIns + 1); // number of values for the range
        this->totalNumCommands = static_cast<int>(minIns) + (rand() % range); // inclusive range [minIns, maxIns]
        // 0 to range-1
    }
    
    //cout << "\nPID: " << this->getPID() << "\n";
    //cout << "numcommands: " << this->totalNumCommands << "\n";

    int instructionBudget = this->totalNumCommands;
    auto cmds = generateRandomCommandList(depth, 1, instructionBudget);
    
    int instructionCount = countNonForInstructions(cmds);
    //cout << "# of generated commands: " << instructionCount << "\n";

    for (auto& c : cmds) addCommand(c);
}

int Process::countNonForInstructions(const vector<shared_ptr<Command>>& cmds) const {
    int count = 0;
    for (const auto& cmd : cmds) {
        auto forCmd = dynamic_pointer_cast<ForLoopCommand>(cmd);
        if (forCmd) {
            // Recursively count inside the for-loop, multiplied by the number of repeats
            count += forCmd->getRepeats() * countNonForInstructions(forCmd->getCommandList());
        }
        else {
            // Count this instruction
            count++;
        }
    }
    return count;
}

void Process::initializeCommands(const vector<string>& instructions) {
    for (const string& instr : instructions) {
        istringstream iss(instr);
        string cmdType;
        iss >> cmdType;

        // Optional: uppercase for case-insensitive match
        transform(cmdType.begin(), cmdType.end(), cmdType.begin(), ::toupper);

        shared_ptr<Command> cmd = nullptr;

        if (cmdType == "DECLARE") {
            string varName;
            uint16_t value;
            iss >> varName >> value;
            if (varName.empty() || iss.fail()) {
                cout << "Invalid DECLARE instruction: " << instr << endl;
                continue;
            }
            cmd = make_shared<DeclareCommand>(shared_from_this(), varName, value);
        }
        else if (cmdType == "ADD") {
            string destVar, srcVar1, srcVar2;
            iss >> destVar >> srcVar1 >> srcVar2;
            if (destVar.empty() || srcVar1.empty() || srcVar2.empty()) {
                cout << "Invalid ADD instruction: " << instr << endl;
                continue;
            }
            cmd = make_shared<AddCommand>(shared_from_this(), destVar, srcVar1, srcVar2);
        }
        else if (cmdType == "SUBTRACT") {
            string destVar, srcVar1, srcVar2;
            iss >> destVar >> srcVar1 >> srcVar2;
            if (destVar.empty() || srcVar1.empty() || srcVar2.empty()) {
                cout << "Invalid SUBTRACT instruction: " << instr << endl;
                continue;
            }
            cmd = make_shared<SubtractCommand>(shared_from_this(), destVar, srcVar1, srcVar2);
        }
        else if (cmdType == "PRINT") {
            string message;
            getline(iss, message);
            message.erase(0, message.find_first_not_of(" \t"));
            cmd = make_shared<PrintCommand>(shared_from_this(), message);
        }
        else if (cmdType == "SLEEP") {
            uint16_t duration;
            iss >> duration;
            if (iss.fail()) {
                cout << "Invalid SLEEP instruction: " << instr << endl;
                continue;
            }
            cmd = make_shared<SleepCommand>(shared_from_this(), duration);
        }
        else if (cmdType == "FOR") {
            int loopCount;
            iss >> loopCount;
            if (iss.fail() || loopCount < 1) {
                cout << "Invalid FOR instruction: " << instr << endl;
                continue;
            }
            vector<shared_ptr<Command>> nestedCommands;
            cmd = make_shared<ForLoopCommand>(shared_from_this(), nestedCommands, loopCount, delay);
        }
        else if (cmdType == "WRITE") {
           //WriteCommand(shared_ptr<Process> process, uint16_t address, uint16_t memFrame, uint16_t value);
            uint16_t address, memFrame, value;
            iss >> address >> memFrame >> value;
            if (iss.fail()) {
                cout << "Invalid WRITE instruction: " << instr << endl;
                continue;
            }

            cmd = make_shared<WriteCommand>(shared_from_this(), address, memFrame, value);
        }
        else if (cmdType == "READ") {
            // TODO: implement WRITE command later
            // cmd = make_shared<WriteCommand>(/* your params */);
            uint16_t address, memFrame;
            string varName;
            iss >> address >> varName >> memFrame;

            if (iss.fail()) {
                cout << "Invalid READ instruction: " << instr << endl;
                continue;
            }

            cmd = make_shared<ReadCommand>(shared_from_this(), address, varName, memFrame);
        }
        else {
            cout << "Unknown instruction: " << instr << endl;
            continue;
        }

        if (cmd) {
            commandList.push_back(cmd);
        }
    }

}