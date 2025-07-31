#include "DeclareCommand.h"
#include "Command.h"
#include "Process.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <unordered_map>


using namespace std;


DeclareCommand::DeclareCommand(shared_ptr<Process> process, const string& varName, uint16_t value) : Command(process, CommandType::DECLARE) {
	this->process = process;
	this->varName = varName;
	this->value = value;
}

void DeclareCommand::execute() {
	// Check if the variable already exists in the symbol table
	int random = 0;
	auto tableCopy = process->getSymbolTable(); // creates one copy

	while (true) {
		if (tableCopy.find(varName) != tableCopy.end()) {
			
			srand(time(0));
			
			varName.append("_" + to_string(rand()*time(0)/10000 * random));
			random = (rand() + random + time(0)) * 32 %10000;
			
			//cout << varName << endl;
			
			//cerr << "Error: Variable '" << varName << "' already declared in process " << process->getPID() << endl;

		}
		else {
			// Declare the variable in the process's symbol table
			process->allocateVariable(varName, value);
			//cout << "Variable '" << varName << "' declared with value " << value << " in process " << process->getPID() << endl;
			//process->visualizeProcessMemory();
			return;
		}
	}
	
	
	//cout << "Executing DeclareCommand for process " << process->getPID() << ": declaring variable '" << varName << "' with value " << value << endl;
}

