#pragma once
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
		Command(int pid, CommandType commandType);
		CommandType getCommandType();
		virtual void execute();

	protected:
		int pid; // Process ID
		CommandType commandType;
};

inline Command::CommandType Command::getCommandType() {
	return this->commandType;
}

inline Command::Command(int pid, CommandType commandType){
	this->pid = pid;
	this->commandType = commandType;
}
