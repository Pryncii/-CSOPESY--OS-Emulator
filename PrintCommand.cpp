#include "PrintCommand.h"
#include "Command.h"
#include <string>
#include <iostream>
#include <mutex>
#include <random>

using namespace std;
extern mutex coutMutex;

PrintCommand::PrintCommand(shared_ptr<Process> process, const string& message) : Command(process, PRINT) {
	this->message = message;
}

PrintCommand::PrintCommand(shared_ptr<Process> process, const string& value, int i) : Command(process, PRINT) {
    this->message = "Value from: " + std::to_string(process->getSymbolValue(value));
    this->symbolName = value;
    isValue = true;
}

void PrintCommand::execute() {
    string temp;
	//lock_guard<mutex> lock(coutMutex);
	
    /*
    auto symbolTable = process->getSymbolTable();

    if (!symbolTable.empty()) {
        // Generate a random index
        size_t randomIndex = std::rand() % symbolTable.size();
        auto it = symbolTable.begin();
        std::advance(it, randomIndex);

        // Print the random symbol and its value
        cout << "Process " << process->getPID() << " prints: " << message
            << " | Random symbol: " << it->first << " = " << it->second << endl;
    }
    else {
        cout << "Process " << process->getPID() << " prints: " << message
            << " | No symbols in table." << endl;
    }
    */
    if (!isValue) {
        string timestamp;
        time_t now = time(nullptr);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", localtime(&now));
        timestamp = buffer;
        temp = "(" + timestamp + ")" + " Core:" + to_string(process->getCpuCoreID()) + message + process->getName() + "!";
    }
    else {
        // Fetch the value at execution time
        temp = "Value from: " + std::to_string(process->getSymbolValue(symbolName));
    }
    process->addLogLine(temp);
}
    //cout << message;

string PrintCommand::getMessage() const{
    return message;
}