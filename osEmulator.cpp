#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

void initialize(){
    cout << "initialize command recognized. Doing something.\n";
}

void screen(){
    cout << "screen command recognized. Doing something.\n";
}

void scheduler_test(){
    cout << "scheduler-test command recognized. Doing something.\n";
}

void scheduler_stop(){
    cout << "scheduler-stop command recognized. Doing something.\n";
}

void report_util(){
    cout << "report-util command recognized. Doing something.\n";
}

void header(){
    cout << "*------------------------------------------------*\n";
    cout << "Hello, welcome to the CSOPESY emulator!\n";
    cout << "Type 'exit' to quit, 'clear' to clear the screen\n";
    cout << "commands: 'initialize', 'screen', 'scheduler-test', 'scheduler-stop', 'report-util'\n";
    cout << "*------------------------------------------------*\n";
}

void clear(){
    #ifdef _WIN32
        system("cls"); // Windows
    #else
        system("clear"); // Unix/Linux/MacOS
    #endif
}

int main(){

    string command;
    
    do{
        header();
        cout << "Enter command: ";
        getline(cin, command);
        if (command == "clear") {
            clear();
        } else if (command == "initialize") {
            initialize();
        } else if (command == "screen") {
            screen();
        } else if (command == "scheduler-test") {
            scheduler_test();
        } else if (command == "scheduler-stop") {
            scheduler_stop();
        } else if (command == "report-util") {
            report_util();
        } else if (command == "exit") {
            exit(0); 
        } else {
            cout << "Unknown command: " << command << "\n";
        }
    } while (true);
   
    return 0;
}