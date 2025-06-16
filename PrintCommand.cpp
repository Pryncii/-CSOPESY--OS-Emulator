#include "PrintCommand.h"
#include "Command.h"
#include <string>
#include <iostream>

using namespace std;

PrintCommand::PrintCommand(int pid, string& message) : Command(pid, PRINT) {
	this->message = message;
}

void PrintCommand::execute() {
	cout << "Process " << pid << " prints: " << message << endl;
}