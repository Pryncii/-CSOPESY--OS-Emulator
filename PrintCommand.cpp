#include "PrintCommand.h"
#include "Command.h"
#include <string>
#include <iostream>

using namespace std;

PrintCommand::PrintCommand(shared_ptr<Process> process, string& message) : Command(process, PRINT) {
	this->message = message;
}

void PrintCommand::execute() {
	cout << "Process " << process->getPid() << " prints: " << message << endl;
}