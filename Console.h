#include <string>
#include <ctime>
#include <iostream>

using namespace std;

class Console{
    private:
        string processName;
        int currentLine;
        int totalLines;
        string timestamp;

    public:
        Console(const string& name, int totalLines);

        void drawScreen() const;
        void updateLine(int newLine);
        string getName() const;

};