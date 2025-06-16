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
