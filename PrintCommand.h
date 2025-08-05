#pragma once
#include <string>
#include <iostream>
#include "Command.h"
using namespace std;

class PrintCommand : public Command {
public:
	PrintCommand(shared_ptr<Process> process, const string& message);
	PrintCommand(shared_ptr<Process> process, const string& message, uint16_t value);
	PrintCommand(shared_ptr<Process> process, const string& message, const string& varName);
	void pageIn(const string& varName);
	void execute() override;
	string getMessage() const;

	string toString() const override {
		return "[PRINT]";
	}

private:
	string message;
	bool isVar;
	uint16_t value;
	string varName;
};

