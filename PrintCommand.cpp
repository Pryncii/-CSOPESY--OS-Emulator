#include "PrintCommand.h"
#include "Command.h"
#include <string>
#include <iostream>
#include <mutex>

using namespace std;
extern mutex coutMutex;

PrintCommand::PrintCommand(shared_ptr<Process> process, const string& message) : Command(process, PRINT) {
	this->message = message;
}

void PrintCommand::execute() {
	//lock_guard<mutex> lock(coutMutex);
	cout << "Process " << process->getPID() << " prints: " << message << endl;
}