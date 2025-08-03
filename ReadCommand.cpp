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

void ReadCommand::execute() {
	//receive the address
	uint16_t pageIndex = address / memFrame;

	if (process->getMemReq() < address){
		cout << "Read Error" << endl;
		//terminate the process if the frame is not allocated
		process->terminateProcess();
		return;
	}

	process->readMemory(pageIndex, address, varName);

}

