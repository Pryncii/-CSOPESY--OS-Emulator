#include <iostream>
#include <string>
#include <cstdlib>
#include <unordered_map>
#include "Console.h"

using namespace std;

std::unordered_map<std::string, Console> screens;

void initialize(){
    cout << "\x1B[32m\x1B[1minitialize\x1B[22m\x1B[0m command recognized. Doing something.\n";
}

void screen(){
    cout << "\x1B[32m\x1B[1mscreen\x1B[22m\x1B[0m command recognized. Doing something.\n";
}

void scheduler_test(){
    cout << "\x1B[32m\x1B[1mscheduler-test\x1B[22m\x1B[0m command recognized. Doing something.\n";
}

void scheduler_stop(){
    cout << "\x1B[32m\x1B[1mscheduler-stop\x1B[22m\x1B[0m command recognized. Doing something.\n";
}

void report_util(){
    cout << "\x1B[32m\x1B[1mreport-util\x1B[22m\x1B[0m command recognized. Doing something.\n";
}

// feel free to change the colors
void header(){
    cout << "+==================================================+\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m                                                  \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m          ___ ___  ___  ___ ___ _____   __        \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m         / __/ __|/ _ \\| _ \\ __/ __\\ \\ / /        \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m        | (__\\__ \\ (_) |  _/ _|\\__ \\\\ V /         \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m         \\___|___/\\___/|_| |___|___/ |_|          \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m   ___  ___    ___            _      _            \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m  / _ \\/ __|  | __|_ _ _ _  _| |__ _| |_ ___ _ _  \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m | (_) \\__ \\  | _|| ' ' | || | / _` |  _/ _ \\ '_| \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m  \\___/|___/  |___|_|_|_|__,_|_\\__,_|___\\___/_|   \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m                                                  \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m                   S20 Group 8                    \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m\x1B[1m        Buencamino, Chua, Ruiz, Seperidad         \033[0m|\n";
    cout << "|\x1B[48;5;195m\x1B[38;5;66m                                                  \033[0m|\n";
    cout << "+==================================================+\n";
    cout << "| Welcome! Here are the available commands:        |\n";
    cout << "|   - initialize        - report-util              |\n";
    cout << "|   - screen            - screen -s <name>         |\n";
    cout << "|   - scheduler-test    - screen -r <name>         |\n";
    cout << "|   - scheduler-stop    - exit                     |\n";
    cout << "|   - clear                                        |\n";
    cout << "+==================================================+\n";
}

void clear(){
    system("cls"); // Windows
    header();
}

void screenInterface(string screenName){
    string screenInput = "";
    system("cls");
    screens[screenName].drawScreen(); 
    do{
        cout << "Enter command: ";
        getline(cin, screenInput);

        if (screenInput != "exit") {
            cout << "\x1B[31m\x1B[1mUnknown command:\x1B[22m " << screenInput << ". \x1B[31m\x1B[1m'exit' is the only available command right now.\x1B[0m\n";
            screenInput = screenInput + "\n\x1B[31m\x1B[1mUnknown command:\x1B[22m " + screenInput + ". 'exit' is the only available command right now.\x1B[0m";
        }

        screens[screenName].setStrings(screenInput);
    } while (screenInput != "exit");
    clear();
}

bool inScreenMap(string name)
{
    // screen name doesn't exist in map
    if (screens.find(name) == screens.end())
        return false;

    return true;
}

int main(){

    string command;
    
    header();
    do{
        string screenName = "";
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
        } else if (command.rfind("screen -s", 0) == 0){
            string rawScreenName;
            rawScreenName = command.substr(9); // get text after -s

            // Trim whitespace
            rawScreenName.erase(0, rawScreenName.find_first_not_of(" \t\n\r")); // left trim (" CSOPESY" becomes "CSOPESY")
            rawScreenName.erase(rawScreenName.find_last_not_of(" \t\n\r") + 1); // right trim ("CSOPESY " becomes "CSOPESY")

            if (rawScreenName.empty()) { // makes sure theres a proper screen name
                std::cout << "\x1B[31m\x1B[1mError:\x1B[0m Screen name cannot be empty.\n";
            }
            else {
                screenName = rawScreenName;
                if (inScreenMap(screenName) == false) { // ensures screen name doesn't exist yet
                    Console temp(screenName);
                    screens.insert({ screenName, temp });
                    screenInterface(screenName);
                }
                else {
                    std::cout << "\x1B[31m\x1B[1mError:\x1B[0m Screen name already exist! Use 'screen -r <process name>' to view the screen.\n";
                }
            }
        } else if (command.rfind("screen -r", 0) == 0){
            string rawScreenName;
            rawScreenName = command.substr(9);

            // Trim whitespace
            rawScreenName.erase(0, rawScreenName.find_first_not_of(" \t\n\r"));
            rawScreenName.erase(rawScreenName.find_last_not_of(" \t\n\r") + 1);

            if (rawScreenName.empty()) { // makes sure theres a proper screen name
                std::cout << "\x1B[31m\x1B[1mError:\x1B[0m Screen name cannot be empty.\n";
            }
            else {
                screenName = rawScreenName;
                if (inScreenMap(screenName) == true) { // ensures screen name already exists
                    screenInterface(screenName);
                }
                else {
                    std::cout << "\x1B[31m\x1B[1mError:\x1B[0m Screen name doesn't exist! Use 'screen -s <process name>' to create the screen.\n";
                }
            }
        } else {
            cout << "\x1B[31m\x1B[1mUnknown command:\x1B[22m " << command << "\x1B[0m\n";
        }
    } while (true);
   
    return 0;
}

/* TO DO LIST
    - [DONE] (not sure if need) empty screen name (from "screen -s  ")
    - [DONE] screen -s and -r does the same thing (make sure -s checks it doesnt exist and -r checks it does)
    - (optional for now) main menu text doesnt get saved
*/