#include "Console.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

Console::Console(const string& name, int totalLines)
    : processName(name), totalLines(totalLines), currentLine(0) {
        time_t now = time(nullptr);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", localtime(&now));
        timestamp = buffer;
    }

void Console::drawScreen() const {
    system("cls");
    cout << "---------------- SCREEN: " << processName << " ----------------\n";
    cout << "Process: " << processName << "\n";
    cout << "Instruction Line: " << currentLine << " / " << totalLines << "\n";
    cout << "Created At: " << timestamp << "\n";
    cout << "\nType 'exit' to return to main menu.\n";
    cout << "---------------------------------------------\n";
}

void Console::updateLine(int newLine) {
    if (newLine <= totalLines)
        currentLine = newLine;
}

string Console::getName() const {
    return processName;
}