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
	uint16_t frameOfAddress = address / memFrame;
	vector<size_t> allocatedFrames = process->getAllocatedFrames();
	unordered_map<size_t, vector<int>> allocatedMemory = process->getProcessMemoryRead();
	bool isThereValue = false;
	bool isFrameAllocated = false;

	for (size_t i = 0; i < allocatedFrames.size(); i++) {
		if (allocatedFrames[i] == frameOfAddress) {
			isFrameAllocated = true;
			break;
		}
	}

	if (!isFrameAllocated) {
		cout << "Error" << endl;
	}


	//write the value to the address



}

