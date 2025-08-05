#pragma once
#include "Command.h"
#include <string>
#include <iostream>

class WriteCommand : public Command
{
public:
	WriteCommand(shared_ptr<Process> process, uint16_t address, uint16_t memFrame, const string& varName);
	WriteCommand(shared_ptr<Process> process, uint16_t address, uint16_t memFrame, uint16_t value);
	void execute() override;
	void pageIn();
	bool useVarName = false;

	string toString() const override {
		return "[WRITE]";
	}

private:
	shared_ptr<Process> process;
	uint16_t address;
	uint16_t value;
	uint16_t memFrame;
	string varName;
};

