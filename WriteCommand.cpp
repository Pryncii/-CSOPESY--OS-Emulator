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

void WriteCommand::execute() {
	//receive the address
	uint16_t frameOfAddress = address/memFrame;
	vector<size_t> allocatedFrames = process->getAllocatedFrames();
	bool isFrameAllocated = false;

	//check if the frame is allocated to the process
	for(size_t i = 0; i < allocatedFrames.size(); i++) {
		if (allocatedFrames[i] == frameOfAddress) {
			isFrameAllocated = true;
			break;
		}
	}

	if(!isFrameAllocated) {
		cout << "Write Error" << endl;
		process->terminateProcess();
		return;
	}

	//write the value to the address

	process->writeToMemory(frameOfAddress, address, value);
	
	/*
	cout << "Memory Write [Frame: " << frameOfAddress
		<< "] [Address: 0x" << hex << address
		<< "] = " << dec << value << endl;
		*/



}

