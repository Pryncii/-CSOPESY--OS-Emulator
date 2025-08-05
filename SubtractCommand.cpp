#include "SubtractCommand.h"

using namespace std;

SubtractCommand::SubtractCommand(shared_ptr<Process> process, const string& var1, uint16_t var2, uint16_t var3)
    : Command(process, CommandType::SUBTRACT) {
    this->var1 = var1;
    this->var2Num = var2;
    this->var3Num = var3;
    this->var2IsNumber = true;
    this->var3IsNumber = true;
}

SubtractCommand::SubtractCommand(shared_ptr<Process> process, const string& var1, const string& var2, const string& var3)
    : Command(process, CommandType::SUBTRACT) {
    this->var1 = var1;
    this->var2Str = var2;
    this->var3Str = var3;
    this->var2IsNumber = false;
    this->var3IsNumber = false;
}

SubtractCommand::SubtractCommand(shared_ptr<Process> process, const string& var1, uint16_t var2, const string& var3)
    : Command(process, CommandType::SUBTRACT) {
    this->var1 = var1;
    this->var2Num = var2;
    this->var3Str = var3;
    this->var2IsNumber = true;
    this->var3IsNumber = false;
}

SubtractCommand::SubtractCommand(shared_ptr<Process> process, const string& var1, const string& var2, uint16_t var3)
    : Command(process, CommandType::SUBTRACT) {
    this->var1 = var1;
    this->var2Str = var2;
    this->var3Num = var3;
    this->var2IsNumber = false;
    this->var3IsNumber = true;
}

void SubtractCommand::pageIn(const string& varName) {
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
void SubtractCommand::execute() {
    uint16_t result = 0;
	//cout << "Executing SubtractCommand: " << var1 << " = " << var2Num << " - " << var3Num << endl;
    if (var2IsNumber && var3IsNumber) {
        result = var2Num - var3Num;
        
        process->editVariable(var1, result);
    }
    else if (var2IsNumber && !var3IsNumber) {
        result = var2Num - process->getSymbolValue(var3Str);
		pageIn(var3Str); // Ensure the variable is paged in if it's not a number
        process->editVariable(var1, result);
    }
    else if (!var2IsNumber && var3IsNumber) {
        result = process->getSymbolValue(var2Str) - var3Num;
		pageIn(var2Str); // Ensure the variable is paged in if it's not a number
        process->editVariable(var1, result);
    }
    else {
        result = process->getSymbolValue(var2Str) - process->getSymbolValue(var3Str);
		pageIn(var2Str); // Ensure the first variable is paged in
		pageIn(var3Str); // Ensure the second variable is paged in
        process->editVariable(var1, result);
    }
}