#pragma once

#include "Command.h"
#include <string>
#include <iostream>

using namespace std;

class SleepCommand: public Command {
public:
	SleepCommand(shared_ptr<Process> process, uint16_t X);
	void execute() override;

	string toString() const override {
		return "[SLEEP]";
	}

private:
	shared_ptr<Process> process;
	uint16_t X;
};
