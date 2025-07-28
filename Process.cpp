#include <string>
#include <iostream>
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
#include <fstream>
#include <mutex>

using namespace std;

Process::Process(int pid, string name, uint32_t delay, uint16_t memoryRequired) {
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

    while (instructionBudget >= repeats) {
        Command::CommandType type = static_cast<Command::CommandType>(rand() % 6); // 0 to 5
        shared_ptr<Command> cmd;
       
        switch (type) {
            case Command::DECLARE: {
                // Seeds the random number generator with the current time (in seconds).
                srand(time(0)); // random number from 0-999 for extra unique
                string varName = to_string(pid) + "x" + to_string(depth) + "_" + to_string(rand() % 1000);
                uint16_t value = rand() % 100;
                cmd = make_shared<DeclareCommand>(shared_from_this(), varName, value);
                instructionBudget -= repeats;
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