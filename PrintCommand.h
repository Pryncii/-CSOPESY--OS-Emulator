#pragma once
#include <string>
#include <iostream>
#include "Command.h"
using namespace std;

class PrintCommand : public Command {
public:
	PrintCommand(shared_ptr<Process> process, const string& message);
	PrintCommand(shared_ptr<Process> process, const string& value, const int i);
	void execute() override;
	string getMessage() const;

	string toString() const override {
		return "[PRINT]";
	}

private:
	string message;
	bool isValue;
	uint16_t value;
	string symbolName;

};

