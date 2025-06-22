#pragma once
#include <string>
#include <iostream>
#include "Command.h"
using namespace std;

class PrintCommand : public Command {
public:
	PrintCommand(shared_ptr<Process> process, const string& message);
	void execute() override;
	string getMessage() const;

private:
	string message;


};

