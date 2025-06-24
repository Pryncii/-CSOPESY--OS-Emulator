#include "SleepCommand.h"

#include "Command.h"
#include "Process.h"
#include <string>
#include <iostream>
#include <unordered_map>
#include <thread>


using namespace std;


SleepCommand::SleepCommand(shared_ptr<Process> process, uint16_t X) : Command(process, CommandType::SLEEP) {
	this->process = process;
	this->X = X;
}

void SleepCommand::execute() {
	// Check if the variable already exists in the symbol table
	std::this_thread::sleep_for(chrono::milliseconds(X));
	//cout << "Process " << process->getPID() << " is sleeping for " << X << " milliseconds." << endl;
	// Make it leave the CPU
	this->process->setCpuCoreID(-1);
}

