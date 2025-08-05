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
                FrameEntry& frame = pagingallocator->frameMap[frameIndex];

                // Write values and label with "WRITE"
                frame.memoryContents[indexInPage] = lowByte;
                frame.memoryContents[indexInPage + 1] = highByte;

                frame.memoryContentsVarName[indexInPage] = "WRITE";
                frame.memoryContentsVarName[indexInPage + 1] = "WRITE";
            }
        }
    }

    //cout << "THIS IS WRITE" << endl;
	//visualizeProcessContents();
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

    //cout << "THIS IS READ" << endl;
    //visualizeProcessContents();
}

void Process::terminateProcess() {
    this->isTerminated = true; // Set commandCounter to the total number of commands
    this->isProcessError = true;
    cout << "process error, terminated.\n";
}

bool Process::getIsProcessError() const {
    return isProcessError;
}

void Process::addToPageIndices(size_t pageIndex) {
    if (find(pageIndices.begin(), pageIndices.end(), pageIndex) == pageIndices.end()) {
        pageIndices.push_back(pageIndex);
    }
}
// Searches for two free bytes in the memory to store a variable, marks memory, and updates symbol tables.
void Process::allocateVariable(const string& varName, uint16_t value) {
    
    //cout << processMemory.size();
    for (int i = 0; i < processMemory.size(); i++) {
		// check if i is inside the pageIndices
        
        // Search for two consecutive free spots
        //cout << processMemory.size();
        
        for (size_t j = 0; j + 1 < processMemory[i].size(); ++j) {

            if (!processMemory[i][j] && !processMemory[i][j + 1]) {
                // Allocate the two spots
               

                if (find(pageIndices.begin(), pageIndices.end(), i) == pageIndices.end()) {
                    pagingallocator->AllocatePage(shared_from_this(), i);// Add the page index to the allocator
                    //cout << allocatedFrames.size() << " frames allocated for process " << this->getName() << endl;
                    deletePageIndexFromFile("backingstore.txt", this->getName(), i); // Delete the page entry from the backing store file
                }
                processMemory[i][j] = true;
                processMemory[i][j + 1] = true;
                processMemoryRead[i][j] = static_cast<uint8_t>(value & 0x00FF);        // Low byte
                processMemoryRead[i][j + 1] = static_cast<uint8_t>((value >> 8) & 0xFF); // High byte



                // Save variable info in symbolTable only
                symbolTable[varName] = value;
                memoryNameTableFrame[varName] = i;
				memoryNameTable[varName] = j ; // Store the address in the frame
                
                if (pagingallocator) {
                    const vector<size_t>& frames = this->getAllocatedFrames();
                    if (i < frames.size()) {
                        size_t frameIdx = frames[i];


                        if (pagingallocator->frameMap.count(frameIdx)) {
                            FrameEntry& frame = pagingallocator->frameMap[frameIdx];

                            frame.memoryContents[j] = static_cast<int>(value & 0x00FF);
                            frame.memoryContents[j + 1] = static_cast<int>((value >> 8) & 0x00FF);

                            frame.memoryContentsVarName[j] = varName;
                            frame.memoryContentsVarName[j + 1] = varName;
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
    
   // cout << "IN EDIT VARIABLE" << endl;
    // Check if the variable exists

	//memoryNameTable[varName] this is the address of the variable in the memory

    //check if
        uint16_t pageIndex = memoryNameTable[varName] / memFrame;
        if (find(pageIndices.begin(), pageIndices.end(), pageIndex) == pageIndices.end()) {
            pagingallocator->AllocatePage(shared_from_this(), pageIndex);// Add the page index to the allocator
            //cout << allocatedFrames.size() << " frames allocated for process " << this->getName() << endl;
            deletePageIndexFromFile("backingstore.txt", this->getName(), pageIndex); // Delete the page entry from the backing store file
        }
        //cout << "SANITY CHECK" << endl;
        if (memoryNameTableFrame.find(varName) != memoryNameTableFrame.end() &&
            memoryNameTable.find(varName) != memoryNameTable.end()) {
           // cout << "in here" << endl;
            //visualizeProcessContents();
            size_t frameIdx = memoryNameTableFrame[varName];
            size_t offset = memoryNameTable[varName];

            //cout << "Edit existing variable from " << processMemoryRead[frameIdx][offset] << " to " << newValue << endl;
            // Update value in memory
            processMemoryRead[frameIdx][offset] = static_cast<uint8_t>(newValue & 0x00FF);        // Low byte

            // Optionally update the second spot if it's used as a placeholder
            processMemoryRead[frameIdx][offset + 1] = static_cast<uint8_t>((newValue >> 8) & 0xFF); // High byte

            if (pagingallocator) {
                const vector<size_t>& frames = this->getAllocatedFrames();
                if (frameIdx < frames.size()) {
                    size_t actualFrameIdx = frames[frameIdx];

                    cout << "in here" << endl;
                    if (pagingallocator->frameMap.count(actualFrameIdx)) {

                        FrameEntry& frame = pagingallocator->frameMap[actualFrameIdx];
                        frame.memoryContents[offset] = static_cast<int>(newValue & 0x00FF);
                        frame.memoryContents[offset + 1] = static_cast<int>((newValue >> 8) & 0x00FF);

                        frame.memoryContentsVarName[offset] = varName;
                        frame.memoryContentsVarName[offset + 1] = varName;
                    }

                }
            }
           // cout << "YEAH" << endl;
            // Update the symbol table too
            symbolTable[varName] = newValue;

           // visualizeProcessContents();
        }
    else {
        allocateVariable(varName, newValue);
        //cout << "Variable does not exist yet. Creating..."<<endl;
    }


}

void Process::savePageIndicesToFile(const std::string& filename) const {
    pagingallocator->addPageIn(); // Increment page in count
    std::ofstream outFile(filename, ios::app);
    if (!outFile) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    for (int page : pageIndices) {
        outFile << getName() << "|" << page << std::endl;
    }

    outFile.close();
}

void Process::deletePageIndexFromFile(const std::string& filename, const std::string& processName, uint16_t pageIndex) const {
	pagingallocator->addPageOut(); // Increment page out count
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    std::string targetEntry = processName + "|" + std::to_string(pageIndex);

    while (std::getline(inFile, line)) {
        if (line != targetEntry) {
            lines.push_back(line); // Keep only non-matching lines
        }
    }
    inFile.close();

    std::ofstream outFile(filename, std::ios::trunc);
    if (!outFile) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    for (const std::string& l : lines) {
        outFile << l << std::endl;
    }
}



void Process::addToAllocatedFrames(size_t frameIndex) {
    
    allocatedFrames.push_back(frameIndex);
    
}

// initializes the allocated frames for the process, and optionally deallocates memory.
void Process::setPages(bool deallocate) {
    

    /*
    - When the process is loaded in, all our page indices will be
      in the backing store
    - We will only page in and allocate to the main memory if an
      address is accessed that is not in the main memory
    - So for the backing store, we will need the process ID, page number,
      and the contents of that page, probably the addresses of variable names(so if we add/subtract a variable,
      we'll first check the address associated with the symbol table 
      call the backing store)
    */

    

    //save to backing store text file

	//this->allocatedFrames = frames;

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

    for (int i = 0; i < memoryRequired / memFrame; i++) {
        this->pageIndices.push_back(i);
    }

    savePageIndicesToFile("backingstore.txt");

    this->pageIndices = {};
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
	return commandList.size();
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
		auto cmd = make_shared<DeclareCommand>(shared_from_this(), symbol, 0); // If symbol not found, add it with value 0
        addSymbol(symbol, 0);
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
    else {
        cout << "oh no";
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
    int targetCommands = instructionBudget; // Default to 10 if not set

    if (depth >= 3) {
        return commands;
    }

    static bool seeded = false;
    if (!seeded) {
        srand(time(0));  // Seed only once
        seeded = true;
    }

    while (commands.size() < targetCommands) {
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

				//cout << "Declare: " << varName << " = " << value << endl;
                break;
            }

            case Command::ADD:
            case Command::SUBTRACT: {
                while (symbolTable.size() < 2) {
                    string fillerName = "autoVar_" + to_string(symbolTable.size());
                    cmd = make_shared<DeclareCommand>(shared_from_this(), fillerName, 0);
                    addSymbol(fillerName, 0);
                }

                auto it1 = symbolTable.begin();
                advance(it1, rand() % symbolTable.size());
                auto it2 = symbolTable.begin();
                advance(it2, rand() % symbolTable.size());

                string varName = "x" + to_string(rand() % 1000);
                string varName2 = "x" + to_string(rand() % 1000);
                uint16_t val2 = rand() % 100;          // First generate val2
                uint16_t val1 = val2 + (rand() % 50 + 1); // Ensure val1 > val2


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
                /*
                int looprepeats = 1 + rand() % 4;
                auto nestedCommands = generateRandomCommandList(depth + 1, looprepeats*repeats, instructionBudget);
                
                if (!nestedCommands.empty()) {
                    cmd = make_shared<ForLoopCommand>(shared_from_this(), nestedCommands, looprepeats, delay);
                }
                break;
                */
           
                int looprepeats = 1 + rand() % 4;

                // Recursively generate subcommands (note: increase depth to avoid infinite nesting)
                auto nestedCommands = generateRandomCommandList(depth + 1, looprepeats * repeats, instructionBudget);

                // Append the repeated commands directly to the parent list
                for (int i = 0; i < looprepeats; ++i) {
                    for (auto& subCmd : nestedCommands) {
                        if (commands.size() < targetCommands) {
                            commands.push_back(subCmd);  // ← this is your idea
                            instructionBudget -= repeats;
                        }
                        else {

                            break;
                        }
                    }
                }
                break;
            

            }
            
            case Command::WRITE: {
				//cout << "Generating WRITE command for process " << name << endl;
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
            if (commands.size() < targetCommands) {
                commands.push_back(cmd);
            }
            else {
                break;
            }
        }
	}

    
    return commands;
	//writeLogsToFile(name + "_logs.txt"); // Write logs to file after generating commands
}

void Process::generateCommands(uint32_t minIns, uint32_t maxIns, int depth) {

    if (this->totalNumCommands == -1) {
        int range = static_cast<int>(maxIns - minIns + 1);
        this->totalNumCommands = static_cast<int>(minIns) + (rand() % range);
    }

    int instructionBudget = this->totalNumCommands;
    auto cmds = generateRandomCommandList(depth, 1, instructionBudget);

    // Instead of using countNonForInstructions, just use the actual size
    for (auto& c : cmds) addCommand(c);

    // Set totalNumCommands to the actual number of commands generated
    this->totalNumCommands = static_cast<int>(commandList.size());
}

void Process::removeFromAllocatedFrames(size_t frameIndex) {
    auto it = find(allocatedFrames.begin(), allocatedFrames.end(), frameIndex);
    if (it != allocatedFrames.end()) {
        allocatedFrames.erase(it);
        //reduce the size of the allocated frames
        
		cout << "Removed frame index " << frameIndex << " from allocated frames." << endl;
		//print out remaining allocated frames for the process
		cout << "Remaining allocated frames for process " << name << ": " << allocatedFrames.size() << endl;
    }
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
        }
        else if (cmdType == "ADD" || cmdType == "SUBTRACT") {
            // Format: ADD destVar srcVar1 srcVar2
            string destVar, srcVar1, srcVar2;
            iss >> destVar >> srcVar1 >> srcVar2;
            if (destVar.empty() || srcVar1.empty() || srcVar2.empty()) {
                cout << "Invalid " << cmdType << " instruction: " << trimmedInstr << endl;
                continue;
            }

            bool isSrc1Const = false, isSrc2Const = false;
            uint16_t val1, val2;

            // Parse srcVar1
            try {
                val1 = static_cast<uint16_t>(stoi(srcVar1));
                isSrc1Const = true;
            }
            catch (const exception& e) {
                //if (!isSrc1Const) cout << "not a value" << endl;
            }

            // Parse srcVar2
            try {
                val2 = static_cast<uint16_t>(stoi(srcVar2));
                isSrc2Const = true;
            }
            catch (const exception& e) {
                //if (!isSrc2Const) cout << "not a value" << endl;
            }

            // Create the appropriate command
            if (cmdType == "ADD") {
                if (isSrc1Const && isSrc2Const) {
                    // Both constants
                    cmd = make_shared<AddCommand>(shared_from_this(), destVar, val1, val2);
                }
                else if (isSrc1Const && !isSrc2Const) {
                    // srcVar1 is constant, srcVar2 is variable
                    cmd = make_shared<AddCommand>(shared_from_this(), destVar, val1, srcVar2);
                }
                else if (!isSrc1Const && isSrc2Const) {
                    // srcVar1 is variable, srcVar2 is constant
                    cmd = make_shared<AddCommand>(shared_from_this(), destVar, srcVar1, val2);
                }
                else {
                    // Both are variables
                    cmd = make_shared<AddCommand>(shared_from_this(), destVar, srcVar1, srcVar2);
                }
            }
            else {
                if (isSrc1Const && isSrc2Const) {
                    // Both constants
                    cmd = make_shared<SubtractCommand>(shared_from_this(), destVar, val1, val2);
                }
                else if (isSrc1Const && !isSrc2Const) {
                    // srcVar1 is constant, srcVar2 is variable
                    cmd = make_shared<SubtractCommand>(shared_from_this(), destVar, val1, srcVar2);
                }
                else if (!isSrc1Const && isSrc2Const) {
                    // srcVar1 is variable, srcVar2 is constant
                    cmd = make_shared<SubtractCommand>(shared_from_this(), destVar, srcVar1, val2);
                }
                else {
                    // Both are variables
                    cmd = make_shared<SubtractCommand>(shared_from_this(), destVar, srcVar1, srcVar2);
                }
            }
        }
        else if (cmdType == "SLEEP") {
            // Format: SLEEP duration
            uint8_t duration;
            iss >> duration;
            if (iss.fail()) {
                cout << "Invalid SLEEP instruction: " << trimmedInstr << endl;
                continue;
            }
            cmd = make_shared<SleepCommand>(shared_from_this(), duration);
        }
        else if (trimmedInstr.substr(0, 5) == "PRINT") {
            // Extract content between parentheses
            size_t openParen = trimmedInstr.find('(');
            size_t closeParen = trimmedInstr.find_last_of(')');

            if (openParen != string::npos && closeParen != string::npos && closeParen > openParen) {
                string content = trimmedInstr.substr(openParen + 1, closeParen - openParen - 1);

                // Remove leading/trailing whitespace from content
                content.erase(0, content.find_first_not_of(" \t"));
                content.erase(content.find_last_not_of(" \t") + 1);

                string finalOutput = "";

                // Check if it contains string concatenation (+ operator)
                if (content.find('+') != string::npos) {
                    // Handle concatenation: "Hello World" + variable
                    istringstream ss(content);
                    string token;

                    while (getline(ss, token, '+')) {
                        // Remove whitespace around token
                        token.erase(0, token.find_first_not_of(" \t"));
                        token.erase(token.find_last_not_of(" \t") + 1);

                        if (token.empty()) continue;

                        if (token.front() == '"' && token.back() == '"') {
                            // It's a string literal - remove quotes and add to output
                            finalOutput += token.substr(1, token.length() - 2);
                        }
                        else {
                            // It's a variable - check if it exists in symbol table
                            if (symbolTable.find(token) != symbolTable.end()) {
                                // Variable exists, get its value
                                finalOutput += to_string(symbolTable[token]);
                            }
                            else {
                                // Variable doesn't exist, declare it with value 0
                                auto declareCmd = make_shared<DeclareCommand>(shared_from_this(), token, 0);
                                addCommand(declareCmd);
                                finalOutput += "0";
                            }
                        }
                    }
                }
                else {
                    // No concatenation - single item
                    if (content.front() == '"' && content.back() == '"') {
                        // It's a string literal
                        finalOutput = content.substr(1, content.length() - 2);
                    }
                    else {
                        // It's a variable
                        if (symbolTable.find(content) != symbolTable.end()) {
                            // Variable exists
                            finalOutput = to_string(symbolTable[content]);
                        }
                        else {
                            // Variable doesn't exist, declare it with value 0
                            auto declareCmd = make_shared<DeclareCommand>(shared_from_this(), content, 0);
                            addCommand(declareCmd);
                            finalOutput = "0";
                        }
                    }
                }

                // Create the PrintCommand with the processed output
                cmd = make_shared<PrintCommand>(shared_from_this(), finalOutput);
            }
        }
        else if (cmdType == "FOR") {

        }
        else if (cmdType == "WRITE") {
            string destVar, srcVar;
            iss >> destVar >> srcVar;
            if (destVar.empty() || srcVar.empty()) {
                cout << "Invalid WRITE instruction: " << trimmedInstr << endl;
                continue;
            }

            // Parse srcVar1 and srcVar2 (could be variables or values)
            uint16_t val1;
            try {
                val1 = static_cast<uint16_t>(stoi(srcVar));
            }
            catch (const exception& e) {
                // It's a variable, get its value from symbol table
                if (symbolTable.find(srcVar) != symbolTable.end()) {
                    val1 = symbolTable[srcVar];
                }
                else {
                    // Auto-declare with value 0
                    addSymbol(srcVar, 0);
                    val1 = 0;
                }
            }
            vector<size_t> possibleAddresses;
            //cout << "memoryrequired: " << memoryRequired << endl;
            //cout << "memFrame: " << memFrame << endl;
            for (int i = 0; i < memoryRequired / memFrame; ++i) {
                //for each page index, add all addresses in that frame to the possible addresses

                for (size_t j = 0; j < memFrame - 1; ++j) {
                    possibleAddresses.push_back(i * memFrame + j);
                }
            }
            //cout << "Possible addresses for WRITE: ";
            //for (size_t addr : possibleAddresses) {
            //    cout << addr << " ";
            //}
            //cout << endl;

            size_t destAddress;
            try {
                destAddress = std::stoul(destVar, nullptr, 0); // auto-detects base (hex, dec)
                //cout << "DEST ADDRESS: " << destAddress << endl;
            }
            catch (const std::exception& e) {
                cout << "Invalid WRITE destination address format: " << destVar << endl;
                continue;
            }

            if (std::find(possibleAddresses.begin(), possibleAddresses.end(), destAddress) == possibleAddresses.end()) {
                //cout << "Invalid WRITE address: " << destVar << " is not within allocated memory" << endl;
                terminateProcess();
                continue;
            }

            cmd = make_shared<WriteCommand>(shared_from_this(), static_cast<uint16_t>(destAddress), memFrame, val1);
            cout << "Valid WRITE instruction" << endl;
        }
        else if (cmdType == "READ") {
            string destVar, srcVar;
            iss >> destVar >> srcVar;
            if (destVar.empty() || srcVar.empty()) {
                cout << "Invalid READ instruction: " << trimmedInstr << endl;
                continue;
            }

            // check if destination is in symbol table, if not, declare with 0
            uint16_t val1;
            try {
                val1 = static_cast<uint16_t>(stoi(destVar));
            }
            catch (const exception& e) {
                // It's a variable, get its value from symbol table
                if (symbolTable.find(destVar) != symbolTable.end()) {
                    val1 = symbolTable[destVar];
                }
                else {
                    // Auto-declare with value 0
                    addSymbol(destVar, 0);
                    val1 = 0;
                }
            }
            vector<size_t> possibleAddresses;
            for (int i = 0; i < memoryRequired / memFrame; ++i) {
                for (size_t j = 0; j < memFrame - 1; ++j) {
                    possibleAddresses.push_back(i * memFrame + j);
                }
            }
            //cout << "Possible addresses for READ: ";
            //for (size_t addr : possibleAddresses) {
            //    cout << addr << " ";
            //}
            //cout << endl;

            size_t srcAddress;
            try {
                srcAddress = std::stoul(srcVar, nullptr, 0); // auto-detects base (hex, dec)
                //cout << "DEST ADDRESS: " << srcAddress << endl;
            }
            catch (const std::exception& e) {
                cout << "Invalid WRITE destination address format: " << destVar << endl;
                continue;
            }

            // check if address source is possible
            if (std::find(possibleAddresses.begin(), possibleAddresses.end(), srcAddress) == possibleAddresses.end()) {
                //cout << "Invalid WRITE address: " << destVar << " is not within allocated memory" << endl;
                terminateProcess();
                continue;
            }

            cmd = make_shared<ReadCommand>(shared_from_this(), static_cast<uint16_t>(srcAddress), destVar, memFrame);
            cout << "Valid READ instruction" << endl;
        }
        else {
            cout << "Unknown instruction: " << instr << endl;
            continue;
        }

        if (cmd) {
            addCommand(cmd);
        }
    }
}