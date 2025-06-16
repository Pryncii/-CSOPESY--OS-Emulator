#pragma once

#include "Command.h"
#include <string>
#include <iostream>

using namespace std;

class DeclareCommand : public Command {
public:
	DeclareCommand(int pid, string& varName, uint16_t value);
	void execute() override;

private:
	string varName;
	uint16_t value;

};
