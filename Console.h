#include <string>
#include <ctime>
#include <iostream>
#include "Process.h"

using namespace std;

class Console{
    private:
        shared_ptr<Process> process;
        string timestamp;
        string totalStrings;

    public:
        Console();
        Console(const shared_ptr<Process> process);
        void drawScreen() const;
        string getStrings() const;
        void setStrings(const string input);
};