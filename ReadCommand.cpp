#include "ReadCommand.h"
#include "Command.h"
#include "Process.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <unordered_map>


using namespace std;


ReadCommand::ReadCommand(shared_ptr<Process> process, uint16_t address, const string& varName, uint16_t memFrame) : Command(process, CommandType::READ) {
	this->process = process;
	this->address = address;
	this->memFrame = memFrame;
	this->varName = varName;
}

void ReadCommand::pageIn() {
	//unordered_map<string, uint16_t> varAddressLocations = process->getMemoryNameTable();
	uint16_t pageIndex = address / memFrame;
	vector <size_t> pageIndices = process->getPageIndices(); // Get the page indices of the process
	shared_ptr<PagingAllocator> pagingallocator = process->getPagingAllocator(); // Get the paging allocator instance
	if (find(pageIndices.begin(), pageIndices.end(), pageIndex) == pageIndices.end()) {
		pagingallocator->AllocatePage(process, pageIndex);// Add the page index to the allocator
		//cout << allocatedFrames.size() << " frames allocated for process " << this->getName() << endl;
		process->deletePageIndexFromFile("backingstore.txt", process->getName(), pageIndex); // Delete the page entry from the backing store file
	}
}

void ReadCommand::execute() {
	//receive the address
	uint16_t pageIndex = address / memFrame;

	//cout << process->getCpuCoreID() << "READ: " << process->getName() << " Address: " << address << " varName: " << varName << endl;

	if (process->getMemReq() < address){
		cout << "Read Error" << endl;
		//terminate the process if the frame is not allocated
		process->terminateProcess();
		return;
	}

	//cout << "Reading from address: " << address << endl;
	
	pageIn(); // Ensure the page is in memory before reading
	process->readMemory(pageIndex, address, varName);

}

