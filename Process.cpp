#include <string>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>
#include "Process.h"
#include "Command.h"
#include "PrintCommand.h"
#include "DeclareCommand.h"
#include "AddCommand.h"
#include "SubtractCommand.h"
#include "SleepCommand.h"
#include <fstream>

using namespace std;

Process::Process(int pid, string name) {
    time_t now = time(nullptr);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", localtime(&now));
    this->timestamp = buffer;
	this->pid = pid;
	this->name = name;
	this->commandCounter = 0;
	this->currentState = ProcessState::READY;
}

void Process::addCommand(shared_ptr<Command> command) {
	commandList.push_back(command);
}

int Process::getPID() const {
	return pid; 
}

int Process::getCurLine() const {
	return this->commandCounter; 
}

string Process::getTime() const {
    return timestamp;
}

int Process::getTotalLines() const {
	return this->commandList.size(); 
}

string Process::getName() const {
	return name;
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

int Process :: getCpuCoreID() const {
    return cpuCoreID;
}

void Process::executeCommand() {
	if (commandCounter < commandList.size()) {
		commandList[commandCounter]->execute();
	}
}

void Process::moveToNextLine() {
	commandCounter++;
}

bool Process::isFinished() const {
	return commandCounter >= commandList.size();
}

void Process::addLogLine(const string& logLine) {
    logLines.push_back(logLine);
}

void Process::printLogs() const {
    cout << endl;
    cout << "Process Name: " << name << endl;
	cout << "ID: " << pid << endl;
    cout << "Logs:" << endl;
    for (const auto& log : logLines) {
        cout << log << endl;
    }
}

string Process::saveLogs() {

    string logtemp = "";
    for (const auto& log : logLines) {
        logtemp = logtemp + log + "\n";
    }

    string temp = "\nProcess Name: " + name + "\n"
        + "ID: " + to_string(pid) + "\n"
        + "Logs:\n" + logtemp;

    return temp;
}
//void Process::writeLogsToFile(const string& filename) const {
//    ofstream outFile(filename);
//    if (!outFile) {
//        cerr << "Failed to open file: " << filename << endl;
//        return;
//    }
//    outFile << "Process Name: " << name << endl;
//    outFile << "Logs:" << endl;
//    for (const auto& cmd : commandList) {
//        // Only log PrintCommand messages
//        auto printCmd = dynamic_pointer_cast<PrintCommand>(cmd);
//        if (printCmd) {
//            outFile << printCmd->getMessage() << endl;
//        }
//    }
//    outFile.close();
//}
	
void Process::generateCommands(const uint32_t minIns, const uint32_t maxIns) {
	int range = static_cast<int>(maxIns - minIns + 1);
    int numCommands = static_cast<int>(minIns) + (rand() % range); // inclusive range [minIns, maxIns]
	
    for (int i = 0; i < numCommands; ++i) {
       Command::CommandType type = static_cast<Command::CommandType>(rand() % 4); // 0 to 5
       
        shared_ptr<Command> cmd;
        /*string toPrint = " Hello World from: ";
        cmd = make_shared<PrintCommand>(shared_from_this(), toPrint);*/
		
        switch (type) {
            case Command::DECLARE: {
                string varName = "x" + to_string(i);
                uint16_t value = rand() % 100;
                cmd = make_shared<DeclareCommand>(shared_from_this(), varName, value);
                break;
            }

            case Command::ADD: {
                while (symbolTable.size() < 2) {
                    string fillerName = "autoVar_" + to_string(symbolTable.size());
                    addSymbol(fillerName, 0);
                }

                auto it1 = symbolTable.begin();
                advance(it1, rand() % symbolTable.size());
                auto it2 = symbolTable.begin();
                advance(it2, rand() % symbolTable.size());

                string varName = "x" + to_string(i);
                uint16_t val1 = it1->second;
                uint16_t val2 = it2->second;

                cmd = make_shared<AddCommand>(shared_from_this(), varName, val1, val2);
                break;
            }

            case Command::SUBTRACT: {
                while (symbolTable.size() < 2) {
                    string fillerName = "autoVar_" + to_string(symbolTable.size());
                    addSymbol(fillerName, 0);
                }

                auto it1 = symbolTable.begin();
                advance(it1, rand() % symbolTable.size());
                auto it2 = symbolTable.begin();
                advance(it2, rand() % symbolTable.size());

                string varName = "x" + to_string(i);
                uint16_t val1 = it1->second;
                uint16_t val2 = it2->second;

                cmd = make_shared<SubtractCommand>(shared_from_this(), varName, val1, val2);
                break;
            }

            case Command::PRINT: {
                cmd = make_shared<PrintCommand>(shared_from_this(), " Hello World from: ");
                break;
            }

            case Command::SLEEP: {
                /*uint16_t value = rand() % 100;
                cmd = make_shared<SleepCommand>(shared_from_this(), value);*/
                break;
            }

            case Command::FOR: {
                // put FOR code here
                break;
            }
        }

        if (cmd) {
            addCommand(cmd);
        }
	}
	//writeLogsToFile(name + "_logs.txt"); // Write logs to file after generating commands
}