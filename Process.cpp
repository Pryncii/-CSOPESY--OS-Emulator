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
#include "PagingAllocator.h"


using namespace std;

Process::Process(int pid, string name, uint32_t delay, uint16_t memoryRequired, uint16_t memFrame, uint16_t maxMem, uint32_t quantum, shared_ptr<PagingAllocator> pagingallocator) {
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
    this->quantum = quantum;
    //cout << (pagingallocator ? "Valid allocator passed\n" : "Allocator is null!\n");
    this->pagingallocator = pagingallocator;
    
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

void Process::writeToMemory(uint16_t pageIndex, uint16_t address, uint16_t value) {
    uint16_t indexInPage = address - (pageIndex * memFrame);

	processMemory[pageIndex][indexInPage] = true; // Mark the memory as used
    processMemory[pageIndex][indexInPage + 1] = true;

	processMemoryRead[pageIndex][indexInPage] = static_cast<uint8_t>(value & 0x00FF);
    processMemoryRead[pageIndex][indexInPage + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);// Store the value in the read memory
    //visualizeProcessMemory();

    if (pagingallocator) {
        //cout << "a";
        const vector<size_t>& frames = this->getAllocatedFrames();
        if (pageIndex < frames.size()) {
            size_t frameIndex = frames[pageIndex];

            int lowByte = static_cast<int>(value & 0x00FF);
            int highByte = static_cast<int>((value >> 8) & 0x00FF);

            if (pagingallocator->frameMap.count(frameIndex)) {
                vector<int>& contents = pagingallocator->frameMap[frameIndex].memoryContents;

                // Make sure both low and high byte indices are within bounds
                if ((indexInPage + 1) < contents.size()) {
                    contents[indexInPage] = lowByte;   // Low byte first
                    contents[indexInPage + 1] = highByte;  // High byte next
                }
            }
        }
    }

}

void Process::readMemory(uint16_t pageIndex, uint16_t address, const string& varName) {
    uint16_t indexInPage = address - (pageIndex * memFrame);
    int readVariable = processMemoryRead[pageIndex][indexInPage];

    if (readVariable == -1) {
        if (variableCounter < 32) {
            allocateVariable(varName, 0);
            variableCounter++; // Increment the variable counter
        }
        //cout << "Memory at address " << address << " in frame " << frameIndex 
             //<< " is not allocated or has no value, allocating 0." << endl;
    } else {
        if (variableCounter < 32) {
            allocateVariable(varName, readVariable); // Allocate the variable with the read value
            variableCounter++;
        }// Increment the variable counter
        //cout << "Read variable '" << varName << "' with value " << readVariable 
			//<< " from address " << address << " in frame " << frameIndex << "." << endl;
	}

}

void Process::terminateProcess() {
	this->isTerminated = true; // Set commandCounter to the total number of commands
}

// Searches for two free bytes in the memory to store a variable, marks memory, and updates symbol tables.
void Process::allocateVariable(const string& varName, uint16_t value) {
    
    //cout << processMemory.size();
    for (int i = 0; i < processMemory.size(); i++) {
        // Search for two consecutive free spots
        //cout << processMemory.size();
        
        for (size_t j = 0; j + 1 < processMemory[i].size(); ++j) {

            if (!processMemory[i][j] && !processMemory[i][j + 1]) {
                // Allocate the two spots
                processMemory[i][j] = true;
                processMemory[i][j + 1] = true;
                processMemoryRead[i][j] = static_cast<uint8_t>(value & 0x00FF);        // Low byte
                processMemoryRead[i][j + 1] = static_cast<uint8_t>((value >> 8) & 0xFF); // High byte

                // Save variable info in symbolTable only
                symbolTable[varName] = value;
                memoryNameTableFrame[varName] = i;
//!!!!!!!!!!!!                memoryNameTableFrame[varName] = frameIdx;  // Store the frame of the variable
				memoryNameTable[varName] = j ; // Store the address in the frame
                
                if (pagingallocator) {
                    const vector<size_t>& frames = this->getAllocatedFrames();
                    if (i < frames.size()) {
                        size_t frameIdx = frames[i];

                        if (pagingallocator->frameMap.count(frameIdx)) {
                            vector<int>& contents = pagingallocator->frameMap[frameIdx].memoryContents;

                            if ((j + 1) < contents.size()) {
                                contents[j] = static_cast<int>(value & 0x00FF);        // Low byte
                                contents[j + 1] = static_cast<int>((value >> 8) & 0x00FF); // High byte
                            }
                        }
                    }
                }
                return; // Allocation successful
            }
        }
    }

    /*
    cout << "Failed to allocate variable '" << varName << "' with value " << value 
		<< ". Not enough memory available." << endl;
        */
}

// Changes the stored value of an existing variable.
// NOTE: MAINMEMORY DOESNT GET UPDATED WHEN EDIT VARIABLE YET
// NOT SURE HOW ITS SUPPOSED TO BE DONE COS NO LOWBYTE HIGHBYTE
void Process::editVariable(const string& varName, uint16_t newValue) {
    // Check if the variable exists


    if (memoryNameTableFrame.find(varName) != memoryNameTableFrame.end() &&
        memoryNameTable.find(varName) != memoryNameTable.end()) {
        visualizeProcessContents();
        size_t frameIdx = memoryNameTableFrame[varName];
        size_t offset = memoryNameTable[varName];

        cout << "Edit existing variable from " << processMemoryRead[frameIdx][offset] << " to " << newValue << endl;
        // Update value in memory
        processMemoryRead[frameIdx][offset] = static_cast<uint8_t>(newValue & 0x00FF);        // Low byte

        // Optionally update the second spot if it's used as a placeholder
        processMemoryRead[frameIdx][offset + 1] = static_cast<uint8_t>((newValue >> 8) & 0xFF); // High byte

        if (pagingallocator) {
            const vector<size_t>& frames = this->getAllocatedFrames();
            if (frameIdx < frames.size()) {
                size_t actualFrameIdx = frames[frameIdx];
                if (pagingallocator->frameMap.count(actualFrameIdx)) {
                    vector<int>& contents = pagingallocator->frameMap[actualFrameIdx].memoryContents;

                    if (offset + 1 < contents.size()) {
                        contents[offset] = static_cast<int>(newValue & 0x00FF);        // Low byte
                        contents[offset + 1] = static_cast<int>((newValue >> 8) & 0x00FF); // High byte
                    }
                }
            }
        }

        // Update the symbol table too
        symbolTable[varName] = newValue;

        visualizeProcessContents();
    }
    else {
        allocateVariable(varName, newValue);
        //cout << "Variable does not exist yet. Creating..."<<endl;
    }


}

// initializes the allocated frames for the process, and optionally deallocates memory.
void Process::setAllocatedFrames(const vector<size_t>& frames, bool deallocate) {
    
	this->allocatedFrames = frames;

    if (deallocate) {
		processMemory.clear();
    }
    else {
        //cout << "For Process " << this->name << ":\n";
		processMemory.resize(memoryRequired/memFrame);
		processMemoryRead.resize(memoryRequired/memFrame);
        for (int i = 0; i < memoryRequired/memFrame; ++i) {
            processMemory[i] = vector<bool>(memFrame, false);
            //cout << "Frame " << frame << ": ";
		    processMemoryRead[i] = vector<int>(memFrame, -1);
            /*
            for (bool bit : processMemory[frame]) {
                cout << bit;
            }
            */
            //cout << endl;
        }
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

// Prints an X for used memory, . for free, page-by-page.
void Process::visualizeProcessMemory() const {
    //cout << "Process Memory Visualization for Process " << name << " (PID: " << pid << "):" << endl;
    for (int i = 0; i < processMemory.size(); i++) {
        cout << "Page " << i << ":  ";
        for (int j = 0; j < processMemory[i].size(); j++) {
            cout << (processMemory[i][j] ? 'X' : '.');
        }
        cout << endl;

        /*
        for (int bit : processMemoryRead.at(frameIdx)) {
            cout << bit << "|";
        }
        cout << endl;
        */
    }
}

// Prints actual value of memory, page-by-page.
void Process::visualizeProcessContents() const {
    //cout << "Process Memory Visualization for Process " << name << " (PID: " << pid << "):" << endl;
    for (int i = 0; i < processMemory.size(); i++) {
        cout << "Page " << i << ": ";
        /*
        for (bool bit : processMemory.at(frameIdx)) {
            cout << (bit ? 'X' : '.');
        }
        cout << endl;
        */

        
        for (int j = 0; j < processMemoryRead[i].size(); j++) {
            if(processMemoryRead[i][j] == -1) {
                cout << ". ";
            } else {
                cout << processMemoryRead[i][j] << " "; // Print the actual value
			}
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
        Command::CommandType type = static_cast<Command::CommandType>(rand() % 8); // 0 to 7
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
                    uint16_t value = rand() % 10;
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
                // only addresses it can access are the addresses that has the allocated frames
                vector<size_t> possibleAddresses;
                uint16_t address;
                uint16_t value;

                for (int i = 0; i < memoryRequired / memFrame; ++i) {
                    //for each page index, add all addresses in that frame to the possible addresses
                    
                    for (size_t j = 0; j < memFrame - 1; ++j) {
                        possibleAddresses.push_back(i * memFrame + j);
                    }
                }


                if (!possibleAddresses.empty()) {
                    address = possibleAddresses[rand() % possibleAddresses.size()];
                    value = rand() % 100;
                    cmd = make_shared<WriteCommand>(shared_from_this(), address, memFrame, value);
                    instructionBudget -= repeats;
                }// Random value in 0255
                
                break;
            }

            case Command::READ: {
                vector<size_t> possibleAddresses;
                uint16_t address;
                string varName;
              
              
                address = rand() % memoryRequired;
                varName = "x" + to_string(rand() % 999); // e.g., x23, x75

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
        string trimmedInstr = instr;
        // Remove leading/trailing whitespace
        trimmedInstr.erase(0, trimmedInstr.find_first_not_of(" \t"));
        trimmedInstr.erase(trimmedInstr.find_last_not_of(" \t") + 1);

        if (trimmedInstr.empty()) continue;

        shared_ptr<Command> cmd = nullptr;
        istringstream iss(trimmedInstr);
        string cmdType;
        iss >> cmdType;

        if (cmdType == "DECLARE") {
            // Format: DECLARE varName value
            string varName;
            uint16_t value;
            iss >> varName >> value;
            if (varName.empty() || iss.fail()) {
                cout << "Invalid DECLARE instruction: " << trimmedInstr << endl;
                continue;
            }
            cmd = make_shared<DeclareCommand>(shared_from_this(), varName, value);
            cout << "Valid DECLARE instruction" << endl;
        }
        else if (cmdType == "ADD") {
            // Format: ADD destVar srcVar1 srcVar2
            string destVar, srcVar1, srcVar2;
            iss >> destVar >> srcVar1 >> srcVar2;
            if (destVar.empty() || srcVar1.empty() || srcVar2.empty()) {
                cout << "Invalid ADD instruction: " << trimmedInstr << endl;
                continue;
            }

            // Parse srcVar1 and srcVar2 (could be variables or values)
            uint16_t val1, val2;

            // Try to parse srcVar1 as number, if fails, treat as variable
            try {
                val1 = static_cast<uint16_t>(stoi(srcVar1));
            }
            catch (const exception& e) {
                // It's a variable, get its value from symbol table
                if (symbolTable.find(srcVar1) != symbolTable.end()) {
                    val1 = symbolTable[srcVar1];
                }
                else {
                    // Auto-declare with value 0
                    addSymbol(srcVar1, 0);
                    val1 = 0;
                }
            }

            // Try to parse srcVar2 as number, if fails, treat as variable
            try {
                val2 = static_cast<uint16_t>(stoi(srcVar2));
            }
            catch (const exception& e) {
                // It's a variable, get its value from symbol table
                if (symbolTable.find(srcVar2) != symbolTable.end()) {
                    val2 = symbolTable[srcVar2];
                }
                else {
                    // Auto-declare with value 0
                    addSymbol(srcVar2, 0);
                    val2 = 0;
                }
            }

            cmd = make_shared<AddCommand>(shared_from_this(), destVar, val1, val2);
            cout << "Valid ADD instruction" << endl;
        }
        else if (cmdType == "SUBTRACT") {
            // Format: SUBTRACT destVar srcVar1 srcVar2
            string destVar, srcVar1, srcVar2;
            iss >> destVar >> srcVar1 >> srcVar2;
            if (destVar.empty() || srcVar1.empty() || srcVar2.empty()) {
                cout << "Invalid SUBTRACT instruction: " << trimmedInstr << endl;
                continue;
            }

            // Parse srcVar1 and srcVar2 (could be variables or values)
            uint16_t val1, val2;

            // Try to parse srcVar1 as number, if fails, treat as variable
            try {
                val1 = static_cast<uint16_t>(stoi(srcVar1));
            }
            catch (const exception& e) {
                // It's a variable, get its value from symbol table
                if (symbolTable.find(srcVar1) != symbolTable.end()) {
                    val1 = symbolTable[srcVar1];
                }
                else {
                    // Auto-declare with value 0
                    addSymbol(srcVar1, 0);
                    val1 = 0;
                }
            }

            // Try to parse srcVar2 as number, if fails, treat as variable
            try {
                val2 = static_cast<uint16_t>(stoi(srcVar2));
            }
            catch (const exception& e) {
                // It's a variable, get its value from symbol table
                if (symbolTable.find(srcVar2) != symbolTable.end()) {
                    val2 = symbolTable[srcVar2];
                }
                else {
                    // Auto-declare with value 0
                    addSymbol(srcVar2, 0);
                    val2 = 0;
                }
            }

            cmd = make_shared<SubtractCommand>(shared_from_this(), destVar, val1, val2);
            cout << "Valid SUBTRACT instruction" << endl;
        }
        else if (cmdType == "SLEEP") {
            // Format: SLEEP duration
            uint16_t duration;
            iss >> duration;
            if (iss.fail()) {
                cout << "Invalid SLEEP instruction: " << trimmedInstr << endl;
                continue;
            }
            cmd = make_shared<SleepCommand>(shared_from_this(), duration);
            cout << "Valid SLEEP instruction" << endl;
        }
        else if (cmdType == "WRITE") {
            
        }
        else if (cmdType == "READ") {
            
        }
        else if (trimmedInstr.substr(0, 5) == "PRINT") {
            
        }
        else if (cmdType == "FOR") {
            
        }
        else {
            cout << "Unknown instruction: " << trimmedInstr << endl;
            continue;
        }

        if (cmd) addCommand(cmd);
    }
}