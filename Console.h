#include <string>
#include <ctime>
#include <iostream>
#define TOTAL 50
#define CURRENT 0

using namespace std;

class Console{
    private:
        string processName;
        int currentLine = CURRENT;
        int totalLines = TOTAL;
        string timestamp;

    public:
        Console(const string name);
        void drawScreen() const;
        void updateLine(int newLine);
        string getName() const;

};