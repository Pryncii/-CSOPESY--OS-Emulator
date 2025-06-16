#pragma once

#include "Command.h"
#include <string>
#include <iostream>



class AddCommand : public Command {
public:
	AddCommand(shared_ptr<Process> process, const string& var1, uint16_t var2, uint16_t var3);
	AddCommand(shared_ptr<Process> process, const string& var1, const string& var2, const string& var3);
	AddCommand(shared_ptr<Process> process, const string& var1, uint16_t var2, const string& var3);
	AddCommand(shared_ptr<Process> process, const string& var1, const string& var2, uint16_t var3);
	void execute() override;

private:
	string var1;
	string var2Str;
	string var3Str;
	bool var2IsNumber = false;
	bool var3IsNumber = false;
	uint16_t var2Num = 0;
	uint16_t var3Num = 0;
	
};
