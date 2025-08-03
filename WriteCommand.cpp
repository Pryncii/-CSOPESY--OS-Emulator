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
	uint16_t pageIndex = address/memFrame;

	if (process->getMemReq() < address) {
		cout << "Write Error" << endl;
		//terminate the process if the frame is not allocated
		process->terminateProcess();
		return;
	}

	//write the value to the address

	process->writeToMemory(pageIndex, address, value);
	
	/*
	cout << "Memory Write [Frame: " << frameOfAddress
		<< "] [Address: 0x" << hex << address
		<< "] = " << dec << value << endl;
		*/



}

