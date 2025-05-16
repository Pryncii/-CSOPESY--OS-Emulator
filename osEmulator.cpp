#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

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
    cout << "|   - screen            - clear                    |\n";
    cout << "|   - scheduler-test    - exit                     |\n";
    cout << "|   - scheduler-stop                               |\n";
    cout << "+==================================================+\n";
}

// cannot output properly if not utf8 interpreted
// needs to interpret first so that it can output properly or else it'll be garbage values
void pretty_header(){
    cout << "╔══════════════════════════════════════════════════╗\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m                                                  \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m          ___ ___  ___  ___ ___ _____   __        \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m         / __/ __|/ _ \\| _ \\ __/ __\\ \\ / /        \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m        | (__\\__ \\ (_) |  _/ _|\\__ \\\\ V /         \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m         \\___|___/\\___/|_| |___|___/ |_|          \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m   ___  ___    ___            _      _            \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m  / _ \\/ __|  | __|_ _ _ _  _| |__ _| |_ ___ _ _  \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m | (_) \\__ \\  | _|| ' ' | || | / _` |  _/ _ \\ '_| \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m  \\___/|___/  |___|_|_|_|__,_|_\\__,_|___\\___/_|   \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m                                                  \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m                   S20 Group 8                    \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m\x1B[1m        Buencamino, Chua, Ruiz, Seperidad         \033[0m║\n";
    cout << "║\x1B[48;5;195m\x1B[38;5;66m                                                  \033[0m║\n";
    cout << "╠══════════════════════════════════════════════════╣\n";
    cout << "║ Welcome! Here are the available commands:        ║\n";
    cout << "║   • initialize        • report-util              ║\n";
    cout << "║   • screen            • clear                    ║\n";
    cout << "║   • scheduler-test    • exit                     ║\n";
    cout << "║   • scheduler-stop                               ║\n";
    cout << "╚══════════════════════════════════════════════════╝\n";
}

void clear(){
    #ifdef _WIN32
        system("cls"); // Windows
        header();
    #else
        system("clear"); // Unix/Linux/MacOS
        header();
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
            cout << "\x1B[31m\x1B[1mUnknown command:\x1B[22m " << command << "\x1B[0m\n";
        }
    } while (true);
   
    return 0;
}