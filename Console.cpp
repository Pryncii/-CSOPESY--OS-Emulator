#include "Console.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

Console::Console(const shared_ptr<Process> process)
    : process(process), totalStrings("") {
        time_t now = time(nullptr);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", localtime(&now));
        timestamp = buffer;
    }

Console::Console()
    : process(nullptr), totalStrings("") {
        time_t now = time(nullptr);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", localtime(&now));
        timestamp = buffer;
    }

void Console::drawScreen() const {
    cout << "============================================\n";
    cout << "  Process Name: " << this->process->getName()<< "\n";
    cout << "  Current Line/Total Lines: ";
    cout << this->process->getCurLines() << "/" << this->process->getTotalLines() << "\n";
    cout << "  Creation Time: " << timestamp << "\n";
    cout << "============================================\n";
    cout << totalStrings;
}

string Console::getStrings() const{
    return totalStrings;
}

void Console::setStrings(string input){
    totalStrings.append("Enter Command: " + input + "\n");
}