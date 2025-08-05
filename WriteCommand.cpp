#include "WriteCommand.h"
#include "Command.h"
#include "Process.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <unordered_map>


using namespace std;



WriteCommand::WriteCommand(shared_ptr<Process> process, uint16_t address, uint16_t memFrame, uint16_t value) : Command(process, CommandType::WRITE) {
	this->process = process;
	this->address = address;
	this->value = value;
	this->memFrame = memFrame;
}
void WriteCommand::pageIn() {
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
void WriteCommand::execute() {
	//receive the address
	uint16_t pageIndex = address/memFrame;

	//cout << process->getCpuCoreID() << "WRITE: " << process->getName() << " Address: " << address << " Value: " << value << endl;

	if (process->getMemReq() < address) {
		cout << "Write Error" << endl;
		//terminate the process if the frame is not allocated
		process->terminateProcess();
		return;
	}
	pageIn();

	//cout << "Write Command: Writing value " << value << " to address " << address << " in process " << process->getName() << endl;

	//write the value to the address

	process->writeToMemory(pageIndex, address, value);
	
	
	
		



}

