#pragma once
#include <memory>
#include "Process.h"

using namespace std;

class Process;

class Command
{
public:
	enum CommandType {
		PRINT,
		DECLARE,
		ADD,
		SUBTRACT,
		SLEEP,
		FOR
	};
	Command(shared_ptr<Process> process, CommandType commandType); // Declaration of the constructor
	CommandType getCommandType();
	virtual void execute();

protected:
	shared_ptr<Process> process;
	CommandType commandType;
};

inline Command::CommandType Command::getCommandType() {
	return this->commandType;
}

