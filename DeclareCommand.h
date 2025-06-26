#pragma once

#include "Command.h"
#include <string>
#include <iostream>

using namespace std;

class DeclareCommand : public Command {
public:
	DeclareCommand(shared_ptr<Process> process, const string& varName, uint16_t value);
	void execute() override;

	string toString() const override {
    return "[DECLARE]";
}

private:
	shared_ptr<Process> process;
	string varName;
	uint16_t value;

};
