#include "PrintCommand.h"
#include "Command.h"
#include <string>
#include <iostream>
#include <mutex>
#include <random>

using namespace std;
extern mutex coutMutex;

PrintCommand::PrintCommand(shared_ptr<Process> process, const string& message) : Command(process, PRINT) {
	this->message = message;
    this->value = NULL;
    isVar = false;
}

PrintCommand::PrintCommand(shared_ptr<Process> process, const string& message, uint16_t value) : Command(process, PRINT) {
    this->message = message + to_string(value);
    this->value = value;
    isVar = false;
}

PrintCommand::PrintCommand(shared_ptr<Process> process, const string& message, const string& varName) : Command(process, PRINT) {
    this->message = message + to_string(process->getSymbolValue(varName));
    this->varName = varName;
    this->value = process->getSymbolValue(varName);
    isVar = true;
}

void PrintCommand::pageIn(const string& varName) {
    unordered_map<string, uint16_t> varAddressLocations = process->getMemoryNameTable();
    uint16_t pageIndex = varAddressLocations[varName] / process->getMemFrame();
    vector <size_t> pageIndices = process->getPageIndices(); // Get the page indices of the process
    shared_ptr<PagingAllocator> pagingallocator = process->getPagingAllocator(); // Get the paging allocator instance
    if (find(pageIndices.begin(), pageIndices.end(), pageIndex) == pageIndices.end()) {
        pagingallocator->AllocatePage(process, pageIndex);// Add the page index to the allocator
        //cout << allocatedFrames.size() << " frames allocated for process " << this->getName() << endl;
        process->deletePageIndexFromFile("csopesy-backing-store.txt", process->getName(), pageIndex); // Delete the page entry from the backing store file
    }
}

void PrintCommand::execute() {
    string temp;
	//lock_guard<mutex> lock(coutMutex);
	
    /*
    auto symbolTable = process->getSymbolTable();

    if (!symbolTable.empty()) {
        // Generate a random index
        size_t randomIndex = std::rand() % symbolTable.size();
        auto it = symbolTable.begin();
        std::advance(it, randomIndex);

        // Print the random symbol and its value
        cout << "Process " << process->getPID() << " prints: " << message
            << " | Random symbol: " << it->first << " = " << it->second << endl;
    }
    else {
        cout << "Process " << process->getPID() << " prints: " << message
            << " | No symbols in table." << endl;
    }
    */
    if (!isVar) {
        string timestamp;
        time_t now = time(nullptr);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", localtime(&now));
        timestamp = buffer;
        temp = "(" + timestamp + ")" + " Core:" + to_string(process->getCpuCoreID()) + " " + message + " from " + process->getName() + "!";
    }
    else {
        // Fetch the value at execution time
        temp = message;
		pageIn(varName);
    }

    //cout << temp;
    process->addLogLine(temp);
}

string PrintCommand::getMessage() const{
    return message;
}