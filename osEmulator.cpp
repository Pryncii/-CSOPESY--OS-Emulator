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

// feel free to change the colors
void header(){
    cout << "+==================================================+\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m                                                  \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m          ___ ___  ___  ___ ___ _____   __        \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m         / __/ __|/ _ \\| _ \\ __/ __\\ \\ / /        \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m        | (__\\__ \\ (_) |  _/ _|\\__ \\\\ V /         \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m         \\___|___/\\___/|_| |___|___/ |_|          \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m   ___  ___    ___            _      _            \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m  / _ \\/ __|  | __|_ _ _ _  _| |__ _| |_ ___ _ _  \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m | (_) \\__ \\  | _|| ' ' | || | / _` |  _/ _ \\ '_| \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m  \\___/|___/  |___|_|_|_|__,_|_\\__,_|___\\___/_|   \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m                                                  \033[0m|\n";
    cout << "+--------------------------------------------------+\n";
    cout << "| Welcome! Here are the commands:                  |\n";
    cout << "| -  'initialize', 'screen', 'scheduler-test',     |\n";
    cout << "| -  'scheduler-stop', 'report-util', 'clear',     |\n";
    cout << "| -  'exit'                                        |\n";
    cout << "|                                                  |\n";
    cout << "| By: CSOPESY S20 Group 8                          |\n";
    cout << "| Buencamino, Chua, Ruiz, Seperidad                |\n";
    cout << "+==================================================+\n";
}

void clear(){
    #ifdef _WIN32
        system("cls"); // Windows
        header();
    #else
        system("clear"); // Unix/Linux/MacOS
        header()
    #endif
}

int main(){

    string command;
    
    header();
    do{
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