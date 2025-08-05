#pragma once
#include <memory>
#include "Process.h"
#include "PagingAllocator.h"

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
		FOR,
		WRITE,
		READ
	};
	Command(shared_ptr<Process> process, CommandType commandType); // Declaration of the constructor
	CommandType getCommandType();
	virtual void execute();

	virtual string toString() const {
		return "[Command]";
	}

protected:
	shared_ptr<Process> process;
	CommandType commandType;
};

inline Command::CommandType Command::getCommandType() {
	return this->commandType;
}

