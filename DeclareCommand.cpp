#include "DeclareCommand.h"
#include "Command.h"
#include <string>
#include <iostream>
#include <unordered_map>



using namespace std;
extern unordered_map<string, uint16_t> symbolTable;

DeclareCommand::DeclareCommand(int pid, string& varName, uint16_t value) : Command(pid, CommandType::DECLARE) {
	this->pid = pid;
	this->varName = varName;
	this->value = value;
}

void DeclareCommand::execute() {
	symbolTable[varName] = move(value);
	cout << "Executing DeclareCommand for process " << pid << ": declaring variable '" << varName << "' with value " << value << endl;
}

