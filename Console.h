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
        string totalStrings;

    public:
        Console();
        Console(const string name);
        void drawScreen() const;
        string getName() const;
        string getStrings() const;
        void setStrings(const string input);
};