#include "Console.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

Console::Console(const string name)
    : processName(name) {
        time_t now = time(nullptr);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", localtime(&now));
        timestamp = buffer;
    }

void Console::drawScreen() const {
    cout << "Process Name: " << processName;
    cout << currentLine << "/" << totalLines;
    cout << "Creation Time: " << timestamp;
  
}

string Console::getName() const{
    return processName;
}
