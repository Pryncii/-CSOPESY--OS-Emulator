#pragma once
#include "Process.h"
#include <memory>
using namespace std;

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
		Command(shared_ptr<Process> process, CommandType commandType);
		CommandType getCommandType();
		virtual void execute();

	protected:
		shared_ptr<Process> process; // Process ID
		CommandType commandType;
};

inline Command::CommandType Command::getCommandType() {
	return this->commandType;
}

inline Command::Command(shared_ptr<Process> process, CommandType commandType){
	this->process = process;
	this->commandType = commandType;
}
