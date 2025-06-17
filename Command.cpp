#include "Command.h"
#include <memory>
#include "Process.h"

// Definition of the constructor
Command::Command(shared_ptr<Process> process, CommandType commandType) {
	this->process = process;
	this->commandType = commandType;
}

void Command::execute() {
 
}

