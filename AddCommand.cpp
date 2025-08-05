#include "AddCommand.h"

using namespace std;


AddCommand::AddCommand(shared_ptr<Process> process, const string& var1, uint16_t var2, uint16_t var3) : Command(process, CommandType::ADD) {
	
	this->var1 = var1;
	this->var2Num = var2;
	this->var3Num = var3;
	this->var2IsNumber = true;
	this->var3IsNumber = true;

}

AddCommand::AddCommand(shared_ptr<Process> process, const string& var1, const string& var2, const string& var3) : Command(process, CommandType::ADD) {
	this->var1 = var1;
	this->var2Str = var2;
	this->var3Str = var3;
	this->var2IsNumber = false;
	this->var3IsNumber = false;
}

AddCommand::AddCommand(shared_ptr<Process> process, const string& var1, uint16_t var2, const string& var3) : Command(process, CommandType::ADD) {
	this->var1 = var1;
	this->var2Num = var2;
	this->var3Str = var3;
	this->var2IsNumber = true;
	this->var3IsNumber = false;
	
}

AddCommand::AddCommand(shared_ptr<Process> process, const string& var1, const string& var2, uint16_t var3) : Command(process, CommandType::ADD) {
	this->var1 = var1;
	this->var2Str = var2;
	this->var3Num = var3;
	this->var2IsNumber = false;
	this->var3IsNumber = true;
	
}
void AddCommand::pageIn(const string& varName) {
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

void AddCommand::execute() {
	// if var 2 and 3 are numbers
	uint16_t result = 0;
	if (var2IsNumber && var3IsNumber) {
		result = var2Num + var3Num;

		//cout << process->getCpuCoreID() << "ADD NN: " << var2Num << " + " << var3Num << " = " << result << endl;


		process->editVariable(var1, result);
	}
	else if (var2IsNumber && !var3IsNumber) { // if var 2 is number and var 3 is string
		result = var2Num + process->getSymbolValue(var3Str);

		//cout << process->getCpuCoreID() << "ADD NV: " << var2Num << " + " << process->getSymbolValue(var3Str) << " = " << result << endl;


		process->editVariable(var1, result);
	}
	else if (!var2IsNumber && var3IsNumber) { // if var 2 is string and var 3 is number
		result = process->getSymbolValue(var2Str) + var3Num;

		//cout << process->getCpuCoreID() << "ADD VN: " << process->getSymbolValue(var2Str) << " + " << var3Num << " = " << result << endl;


		process->editVariable(var1, result);
	}
	else { // if both are strings
		result = process->getSymbolValue(var2Str) + process->getSymbolValue(var3Str);

		//cout << process->getCpuCoreID() << "ADD VV: " << process->getSymbolValue(var2Str) << " + " << process->getSymbolValue(var3Str) << " = " << result << endl;


		process->editVariable(var1, result);
	}

}
