#include "Command.h"
#include <memory>

Command::Command(shared_ptr<Process> process, CommandType commandType) {
	this->process = process;
	this->commandType = commandType;
}

void Command::execute() {
 
}

