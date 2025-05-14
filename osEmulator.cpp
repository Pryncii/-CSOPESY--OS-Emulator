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
    cout << "+=====================================================+\n";
    cout << "|\x1B[48;5;109m                                                     \033[0m|\n";
    cout << "|\x1B[48;5;109m     \x1B[38;5;66m _____ _____ _____ _____ _____ _____ __ __      \033[0m|\n";
    cout << "|\x1B[48;5;109m     \x1B[38;5;66m\x1B[48;5;193m|     |   __|     |  _  |   __|   __|  |  |\x1B[48;5;109m     \033[0m|\n";
    cout << "|\x1B[48;5;109m     \x1B[38;5;66m\x1B[48;5;229m|   --|__   |  |  |   __|   __|__   |_   _|\x1B[48;5;109m     \033[0m|\n";
    cout << "|\x1B[48;5;109m     \x1B[38;5;66m\x1B[48;5;223m|_____|_____|_____|__|\x1B[48;5;109m  \x1B[38;5;66m\x1B[48;5;223m|_____|_____|\x1B[48;5;109m \x1B[38;5;66m\x1B[48;5;223m|_|\x1B[48;5;109m       \033[0m|\n";
    cout << "|\x1B[48;5;109m \x1B[38;5;66m _____ _____     _____           _     _            \033[0m|\n";
    cout << "|\x1B[48;5;109m \x1B[38;5;66m\x1B[48;5;193m|     |   __|\x1B[48;5;109m   \x1B[38;5;66m\x1B[48;5;193m|   __|\x1B[48;5;109m\x1B[38;5;66m_____ _ _\x1B[48;5;109m\x1B[38;5;66m\x1B[48;5;193m| |\x1B[48;5;109m\x1B[38;5;66m___\x1B[48;5;109m\x1B[38;5;66m\x1B[48;5;193m| |\x1B[48;5;109m\x1B[38;5;66m_ ___ ___\x1B[48;5;109m  \033[0m|\n";
    cout << "|\x1B[48;5;109m \x1B[38;5;66m\x1B[48;5;229m|  |  |__   |\x1B[48;5;109m   \x1B[38;5;66m\x1B[48;5;229m|   __|     | | | | .'|  _| . |  _|\x1B[48;5;109m \033[0m|\n";
    cout << "|\x1B[48;5;109m \x1B[38;5;66m\x1B[48;5;223m|_____|_____|\x1B[48;5;109m   \x1B[38;5;66m\x1B[48;5;223m|_____|_|_|_|___|_|__,|_|\x1B[48;5;109m \x1B[38;5;66m\x1B[48;5;223m|___|_|\x1B[48;5;109m   \033[0m|\n";
    cout << "|\x1B[48;5;109m                                                     \033[0m|\n";
    cout << "|\x1B[48;5;109m                                                     \033[0m|\n";
    cout << "+-----------------------------------------------------+\n";
    cout << "| Welcome! Here are the commands:                     |\n";
    cout << "| -  'initialize', 'screen', 'scheduler-test',        |\n";
    cout << "| -  'scheduler-stop', 'report-util', 'clear',        |\n";
    cout << "| -  'exit'                                           |\n";
    cout << "|                                                     |\n";
    cout << "| By: CSOPESY S20 Group 8                             |\n";
    cout << "| Buencamino, Chua, Ruiz, Seperidad                   |\n";
    cout << "+=====================================================+\n";



    //cout << "Hello, welcome to the CSOPESY emulator!\n";
    //cout << "Type 'exit' to quit, 'clear' to clear the screen\n";
    //cout << "commands: 'initialize', 'screen', 'scheduler-test', 'scheduler-stop', 'report-util'\n";
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