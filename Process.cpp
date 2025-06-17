#include "Process.h"
#include <string>
#include <iostream>
#include <memory>
#include "Command.h"

using namespace std;

Process::Process(int pid, string name) {
	this->pid = pid;
	this->name = name;
	this->commandCounter = 0;
	this->currentState = ProcessState::READY;

}

void Process::addCommand(shared_ptr<Command> command) {
	commandList.push_back(command);
}


int Process::getPid() const {
	return pid; 
}

void Process::setCpuCoreID(int cpuCoreID) {
	this->cpuCoreID = cpuCoreID;
}

void Process::addSymbol(const string& symbol, uint16_t value) {
	symbolTable[symbol] = value;
}

unordered_map<string, uint16_t> Process::getSymbolTable() const {
	return symbolTable;
}

uint16_t Process::getSymbolValue(const string& symbol) {
	if (symbolTable.find(symbol) == symbolTable.end()) {
		addSymbol(symbol, 0); // If symbol not found, add it with value 0
	}

	return symbolTable.at(symbol);
}

	