#include "Process.h"
#include <string>
#include <iostream>

using namespace std;

Process::Process(int pid, string name) {
	this->pid = pid;
	this->name = name;
	this->commandCounter = 0;
	this->currentState = ProcessState::READY;

}

void Process::addCommand(string command) {
	commandList.push_back(command);
}

void Process::executeCommand() {
	


}

int Process::getPid() const {
	return pid; 
}

void Process::addSymbol(const string& symbol, uint16_t value) {
	symbolTable[symbol] = value;
}

unordered_map<string, uint16_t> Process::getSymbolTable() const {
	return symbolTable;
}

uint16_t Process::getSymbolValue(const string& symbol) const {
	return symbolTable.at(symbol);
}

	