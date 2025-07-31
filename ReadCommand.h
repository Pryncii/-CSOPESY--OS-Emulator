#pragma once
#include "Command.h"
#include <string>
#include <iostream>

class ReadCommand : public Command
{
public:
	ReadCommand(shared_ptr<Process> process, uint16_t address, const string& varName, uint16_t memFrame);
	void execute() override;

	string toString() const override {
		return "[READ]";
	}

private:
	shared_ptr<Process> process;
	uint16_t address;
	uint16_t memFrame;
	string varName;
};

