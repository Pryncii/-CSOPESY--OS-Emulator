#include "Console.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#define TOTAL 500
#define CURRENT 0
using namespace std;

Console::Console(const string name)
    : processName(name), currentLine(CURRENT), totalLines(TOTAL), totalStrings("") {
        time_t now = time(nullptr);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", localtime(&now));
        timestamp = buffer;
    }

Console::Console()
    : processName(""), currentLine(CURRENT), totalLines(TOTAL), totalStrings("") {
        time_t now = time(nullptr);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", localtime(&now));
        timestamp = buffer;
    }

void Console::drawScreen() const {
    cout << "============================================\n";
    cout << "| Process Name: " << processName << "                        |\n";
    cout << "| Current Line/Total Lines: ";
    cout << currentLine << "/" << totalLines << "          |\n";
    cout << "| Creation Time: " << timestamp << "   |\n";
    cout << "============================================\n";
    cout << totalStrings;
}

string Console::getName() const{
    return processName;
}

string Console::getStrings() const{
    return totalStrings;
}

void Console::setStrings(string input){
    totalStrings.append("Enter Command: " + input + "\n");
}
