#pragma once
#pragma once

#include "Command.h"
#include <string>
#include <iostream>

using namespace std;


class AddCommand : public Command {
public:
	AddCommand(int pid, string& varName, uint16_t value);
	void execute() override;

private:
	string varName1;
	string varName2;
	string varName3;
};
