#include "DeclareCommand.h"
#include "Command.h"
#include "Process.h"
#include <string>
#include <iostream>
#include <unordered_map>


using namespace std;


DeclareCommand::DeclareCommand(shared_ptr<Process> process, const string& varName, uint16_t value) : Command(process, CommandType::DECLARE) {
	this->process = process;
	this->varName = varName;
	this->value = value;
}

void DeclareCommand::execute() {
	// Check if the variable already exists in the symbol table
	
	auto tableCopy = process->getSymbolTable(); // creates one copy

	if (tableCopy.find(varName) != tableCopy.end()) {
		cerr << "Error: Variable '" << varName << "' already declared in process " << process->getPID() << endl;
		return;
	}
	else {
			// Declare the variable in the process's symbol table
		process->addSymbol(varName, value);
	}
	
	//cout << "Executing DeclareCommand for process " << process->getPID() << ": declaring variable '" << varName << "' with value " << value << endl;
}

