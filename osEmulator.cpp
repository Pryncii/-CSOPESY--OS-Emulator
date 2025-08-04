#include <iostream>
#include <string>
#include <cstdlib>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <thread>
#include "Console.h"
#include "Scheduler.h"
#include "PrintCommand.h"
#include "SleepCommand.h"
#include "ForLoopCommand.h"
#include "SubtractCommand.h"
#include <algorithm> 
#include <random>
#include "FlatMemoryAllocator.h"
#include "PagingAllocator.h"

using namespace std;

unordered_map<string, shared_ptr<Console>> screens;
int cpuCycles = 0;
int globalPID = 1000;
bool processGeneration = false;
int processCount = 0;
string consoleStrings = "";

struct Config {
    int numCPU; // 1-128
    string schedulerMode;
    uint32_t quantum; // 1-2^32
    uint32_t batchFreq; // 1-2^32
    uint32_t minIns; // 1-2^32
    uint32_t maxIns; // 1-2^32
    uint32_t delay; // 0-2^32
    uint16_t maxMem;
	uint16_t memFrame; // 1-2^16
	uint16_t minMemProc; // 1-2^16
	uint16_t maxMemProc; // 1-2^16
};

shared_ptr<PagingAllocator> pagingallocator;

bool isNonNegativeInteger(const string& s) {
    if (s.empty()) return false;

    for (char c : s) {
        if (!isdigit(c)) return false;
    }

    return true;
}

uint32_t parsecheckWithinRange(const string& stringvalue, const string& key, uint32_t min, uint32_t max) {
    if (!isNonNegativeInteger(stringvalue)) {
        cerr << "Invalid value for " << key << ": \"" << stringvalue << "\" (must be a non-negative integer)\n";
        exit(1);
    }

    try {
        uint64_t val = stoull(stringvalue); // used stoull instead of stoi bc its bigger and jic the config has crazy high numbers
        if (val < min || val > max) { // if outside range
            cerr << "Value for " << key << " out of range [" << min << ", " << max << "]: " << val << "\n";
            exit(1);
        }
        return static_cast<uint32_t>(val); // static_cast just to be safe since val up there is 64 bits
    }
    catch (...) {
        cerr << "Failed to parse value for " << key << ": " << stringvalue << "\n";
        exit(1);
    }
}

Config loadConfig() {
    ifstream file("config.txt");
    if (!file) {
        cout << "Failed to open config.txt\n";
        exit(1);
    }

    unordered_map<string, string> textconfig;
    string line;

    while (getline(file, line)) {
        istringstream iss(line); // breaking down line into words
        string key, value;
        if (iss >> key >> value) { // key here would be the first word (ex. num-cpu), value will be second word (ex. 4)
            textconfig[key] = value;
        }
    }

    Config config;

    if (textconfig.find("scheduler") == textconfig.end()) {
        cout << "Missing required key: scheduler\n";
        exit(1);
    }
    config.schedulerMode = textconfig["scheduler"];
    config.schedulerMode.erase(remove(config.schedulerMode.begin(), config.schedulerMode.end(), '"'), config.schedulerMode.end()); // remove quotes ("rr" --> rr)

    // check that its only either rr or fcfs
    if (config.schedulerMode != "fcfs" && config.schedulerMode != "rr") {
        cerr << "Invalid scheduler: " << config.schedulerMode << " (must be \"fcfs\" or \"rr\")\n";
        exit(1);
    }

    const vector<string> checkIntKeys = {
        "num-cpu", "quantum-cycles", "batch-process-freq", "min-ins", "max-ins", "delay-per-exec"
    };

    for (const string& key : checkIntKeys) {
        if (textconfig.find(key) == textconfig.end()) {
            cout << "Missing required key: " << key << "\n";
            exit(1);
        }
        if (!isNonNegativeInteger(textconfig[key])) {
            cout << "Invalid value for " << key << ": " << textconfig[key]
                << " (must be a positive integer)\n";
            exit(1);
        }
    }

    // Parse all integer values with proper ranges
    config.numCPU = parsecheckWithinRange(textconfig["num-cpu"], "num-cpu", 1, 128);
    config.quantum = parsecheckWithinRange(textconfig["quantum-cycles"], "quantum-cycles", 1, UINT32_MAX);
    config.batchFreq = parsecheckWithinRange(textconfig["batch-process-freq"], "batch-process-freq", 1, UINT32_MAX);
    config.minIns = parsecheckWithinRange(textconfig["min-ins"], "min-ins", 1, UINT32_MAX);
    config.maxIns = parsecheckWithinRange(textconfig["max-ins"], "max-ins", 1, UINT32_MAX);
    config.delay = parsecheckWithinRange(textconfig["delay-per-exec"], "delay-per-exec", 0, UINT32_MAX);
	config.maxMem = parsecheckWithinRange(textconfig["max-overall-mem"], "max-overall-meme", 64, UINT16_MAX);
	config.memFrame = parsecheckWithinRange(textconfig["mem-per-frame"], "mem-per-frame", 0, UINT16_MAX);
	config.minMemProc = parsecheckWithinRange(textconfig["min-mem-per-proc"], "min-mem-per-proc", 64, UINT16_MAX);
	config.maxMemProc = parsecheckWithinRange(textconfig["max-mem-per-proc"], "max-mem-per-proc", 64, UINT16_MAX);

    return config;
}

uint16_t generateMem(Config config) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint16_t> memDist(config.minMemProc, config.maxMemProc);
    uint16_t memoryRequired = memDist(gen);

    return memoryRequired;
}

void scheduler_start(Config config, Scheduler& scheduler){
    //cout << "\x1B[32m\x1B[1mscheduler-start\x1B[22m\x1B[0m command recognized. Generating Processes.\n";
    processGeneration = true;
    while (processGeneration) {
        // Generate a new process second
		// generate a new process every batchFreq cycles
        this_thread::sleep_for(chrono::milliseconds(1));
        if (cpuCycles % config.batchFreq == 0) {

            
            uint16_t memoryRequired = generateMem(config);

            shared_ptr<Process> process = make_shared<Process>(globalPID, "Process_" + to_string(globalPID), config.delay, memoryRequired, config.memFrame, config.maxMem, config.quantum, pagingallocator);
            //cout << config.quantum << endl;
            //process->generateCommands(config.minIns, config.maxIns, 0);
            scheduler.addProcess(process); // add the process to the scheduler

            Console temp(process);
            shared_ptr<Console> consolePtr = make_shared<Console>(temp);
            screens.insert({ "Process_" + to_string(globalPID), consolePtr });
            globalPID++;

            // Create a new process with a unique PID

            //cout << "New process " << newProcess->getName() << " added to the scheduler.\n";
        }
	}
}

void help(){
    cout << "+====================================================================+\n";
    cout << "|                         Available Commands                         |\n";
    cout << "+====================================================================+\n";
    cout << "| SYSTEM COMMANDS                                                    |\n";
    cout << "|   - initialize                 : Set up processor configuration    |\n";
    cout << "|   - exit                       : Exit the application              |\n";
    cout << "|   - clear                      : Clear the console                 |\n";
    cout << "+--------------------------------------------------------------------+\n";
    cout << "| SCREEN COMMANDS                                                    |\n";
    cout << "|   - screen -s <name> <memsize>                  : Create a screen  |\n";
    cout << "|   - screen -c <name> <memsize> \"<instructions>\" : Create with code |\n";
    cout << "|   - screen -r <name>                            : Reattach screen  |\n";
    cout << "|   - screen -ls                                  : List all screens |\n";
    cout << "+--------------------------------------------------------------------+\n";
    cout << "| SCHEDULER COMMANDS                                                 |\n";
    cout << "|   - scheduler-start            : Start the scheduler               |\n";
    cout << "|   - scheduler-stop             : Stop the scheduler                |\n";
    cout << "|   - report-util                : CPU utilization report            |\n";
    cout << "+====================================================================+\n";
}

void report_util(Config config, Scheduler& scheduler){
    cout << "\x1B[32m\x1B[1mreport-util\x1B[22m\x1B[0m command recognized. Generating logs.\n";

    ofstream outFile("csopesy-log.txt");

    if (screens.empty()) {
        outFile << "No screens available.\n";
        return;
    }
    else {
        outFile << "CPU Utilization: " << ((scheduler.getRunningCores() * 100) / (config.numCPU)) << "%\n";
        outFile << "Cores used: " << scheduler.getRunningCores() << "\n";
        outFile << "Cores available: " << config.numCPU - scheduler.getRunningCores() << "\n";

        outFile << "+============================================================+\n";
        outFile << "Running processes:\n";

        vector<shared_ptr<Process>> runningQueueCopy = scheduler.getRunningQueue();
        for (const auto& process : runningQueueCopy) {
            outFile << process->getName() << "    (" << process->getTime() << ")   "
                << " Core: " << process->getCpuCoreID()
                << "    " << process->getCurLine() << "/" << process->getTotalLines() << "\n";
        }

        outFile << "\nFinished processes:\n";
        vector<shared_ptr<Process>> finishedQueueCopy = scheduler.getFinishedQueue();
        for (const auto& process : finishedQueueCopy) {
            outFile << process->getName() << "    (" << process->getTime() << ")   "
                << " Finished"
                << "    " << process->getCurLine() << "/" << process->getTotalLines() << "\n";
        }

        outFile << "+============================================================+\n";
    }

    outFile.close();
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
}

void clear(){
    system("cls"); // Windows
    consoleStrings = "";
    header();
}

void screenInterface(string screenName){
    string screenInput = "process-smi";
    system("cls");
    screens[screenName]->drawScreen(); 
    do {
        cout << "Enter command: ";
        getline(cin, screenInput);


        if (screenInput != "exit" && screenInput != "process-smi") {
            cout << "\x1B[31m\x1B[1mUnknown command:\x1B[22m " << screenInput + "\x1B[0m\n";
            screenInput = screenInput + "\n\x1B[31m\x1B[1mUnknown command:\x1B[22m " + screenInput + "\x1B[0m";
        }
        
        screens[screenName]->setStrings(screenInput);

        if (screenInput == "process-smi") {
			screens[screenName]->getProcess()->printLogs(); // Redraw the screen
            screens[screenName]->setStrings(screens[screenName]->getProcess()->saveLogs());
        }


       
    } while (screenInput != "exit");
	system("cls"); // Clear the screen when exiting
    header();
    cout << consoleStrings;
}

bool inScreenMap(string name)
{
    // screen name doesn't exist in map
    if (screens.find(name) == screens.end())
        return false;

    return true;
}

void cpuCycleThread(uint32_t delayMs) {
    while (true) {
        this_thread::sleep_for(chrono::milliseconds(1));
        ++cpuCycles;
	    //cout << "Cycle: " << cpuCycles << "\n"; // Print CPU cycles every delayMs milliseconds
    }
}

void processSmi(Scheduler& scheduler, Config config) {

    auto cpuUtilization = (scheduler.getRunningCores() * 100) / config.numCPU;
    uint16_t totalMemory = config.maxMem;
	size_t totalMemoryUsed = 0;
	

    vector<shared_ptr<Process>> runningProcesses = scheduler.getRunningQueue();
	queue<shared_ptr<Process>> readyProcesses = scheduler.getReadyQueue();

    for (const shared_ptr<Process>& process : runningProcesses) {
        vector<vector<bool>> curProcMem = process->getProcessMemory();

        for (int i = 0; i < curProcMem.size(); i++) {
            //size_t frameIndex = pair.first;
            //const vector<bool>& frameData = pair.second;

            for (int j = 0; j < curProcMem[i].size(); j++) {
                if (curProcMem[i][j]) {
                    ++totalMemoryUsed;
                }
            }
        }
    }

    queue<shared_ptr<Process>> tempReadyQueue = readyProcesses; // copy
    while (!tempReadyQueue.empty()) {
        shared_ptr<Process> process = tempReadyQueue.front();
        tempReadyQueue.pop();

        vector<vector<bool>> curProcMem = process->getProcessMemory();

        for (int i = 0; i < curProcMem.size(); i++) {
            //const vector<bool>& frameData = pair.second;
            for (int j = 0; j < curProcMem[i].size(); j++) {
                if (curProcMem[i][j]) {
                    ++totalMemoryUsed;
                }
            }
        }
    }


	auto memoryUtilization = (totalMemoryUsed * 100) / totalMemory;

    // For each running process, print its name and memory usage
    cout << "=================================================\n";
    cout << "| PROCESS-SMI V01.00 Driver Version: 01.00 |\n";
    cout << "-------------------------------------------------\n";
    cout << "CPU Utilization: " << cpuUtilization << endl;
	cout << "Memory Usage: " << totalMemoryUsed << " bytes" << "/" << totalMemory << " bytes\n";
	cout << "Memory Utilization: " << memoryUtilization << "%\n" << endl;
    cout << "=================================================\n";
    cout << "Running processes and memory usage:\n";
    cout << "-------------------------------------------------\n";
   
    for (const shared_ptr<Process>& process : runningProcesses) {
        vector<vector<bool>> curProcMem = process->getProcessMemory();

        size_t memoryUsed = 0;

        for (int i = 0; i < curProcMem.size(); i++) {
            //size_t frameIndex = pair.first;
            //const vector<bool>& frameData = pair.second;

            for (int j = 0; j < curProcMem[i].size(); j++) {
                if (curProcMem[i][j]) {
                    ++memoryUsed;
                }
            }
        }

        cout << process->getName() << " (PID: " << process->getPID() << "):"
            << " | Memory Used: " << memoryUsed << " bytes\n";

        process->visualizeProcessContents();
    }

    cout << "\n=================================================\n";
    cout << "Ready queue processes and memory usage:\n";
    cout << "-------------------------------------------------\n";

    tempReadyQueue = readyProcesses; // reset copy for printing
    while (!tempReadyQueue.empty()) {
        shared_ptr<Process> process = tempReadyQueue.front();
        tempReadyQueue.pop();

        vector<vector<bool>> curProcMem = process->getProcessMemory();
        size_t memoryUsed = 0;

        for (int i = 0; i < curProcMem.size(); i++) {
            //const vector<bool>& frameData = pair.second;
            for (int j = 0; j < curProcMem[i].size(); j++) {
                if (curProcMem[i][j]) {
                    ++memoryUsed;
                }
            }
        }

        cout << process->getName() << " (PID: " << process->getPID() << ")"
            << " | Memory Used: " << memoryUsed << " bytes\n";

        process->visualizeProcessContents();

        // Visualize the process memory allocation
    }
}

void saveLs(Scheduler& scheduler, Config config) {
    vector<std::shared_ptr<Process>> runningQueueCopy = scheduler.getRunningQueue();
    vector<std::shared_ptr<Process>> finishedQueueCopy = scheduler.getFinishedQueue();

    string output =
        "CPU Utilization: " + std::to_string((scheduler.getRunningCores() * 100) / config.numCPU) + "%\n" +
        "Cores used: " + std::to_string(scheduler.getRunningCores()) + "\n" +
        "Cores available: " + std::to_string(config.numCPU - scheduler.getRunningCores()) + "\n" +
        "+============================================================+\n" +
        "Running processes:\n";

    std::sort(runningQueueCopy.begin(), runningQueueCopy.end(),
    [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
        return a->getCpuCoreID() < b->getCpuCoreID();
    });

    for (const auto& process : runningQueueCopy) {
        output += process->getName() + "    (" + process->getTime() + ")   " +
            " Core: " + std::to_string(process->getCpuCoreID()) +
            "    " + std::to_string(process->getCurLine()) + "/" + std::to_string(process->getTotalLines()) + "\n";
    }

    output += "\nFinished processes:\n";

    for (const auto& process : finishedQueueCopy) {
        output += process->getName() + "    (" + process->getTime() + ")   " +
            " Finished" +
            "    " + std::to_string(process->getCurLine()) + "/" + std::to_string(process->getTotalLines()) + "\n";
    }

    output += "+============================================================+\n";
    cout << output;
    consoleStrings.append(output);
}

uint16_t isValidMemory(const string& command, uint16_t maxMem) {
    istringstream iss(command);
    string screen, flag, screenName, memStr;

    iss >> screen >> flag >> screenName >> memStr;

    if (!isNonNegativeInteger(memStr)) {
        cout << "Error: Memory value must be a non-negative integer.\n";
        return 0;
    }

    int memVal = stoi(memStr);

    if (memVal < 64 || memVal > maxMem) {
        cout << "invalid memory allocation";
        return 0;
    }

    // Check if memVal is a power of two
    if (!(memVal > 0 && (memVal & (memVal - 1)) == 0)) {
        cout << "Error: Memory must be a power of two.\n";
        return 0;
    }

    return static_cast<uint16_t>(memVal);
}

string getScreenName(const string& command) {
    istringstream iss(command);
    string token, subcommand, screenName;

    iss >> token >> subcommand >> screenName;

    screenName.erase(0, screenName.find_first_not_of(" \t\n\r"));
    screenName.erase(screenName.find_last_not_of(" \t\n\r") + 1);

    return screenName;
}

vector<string> parseInstructionsFromCommand(const string& command) {
    size_t startQuote = command.find('\"');
    size_t endQuote = command.rfind('\"');

    if (startQuote == string::npos || endQuote == string::npos || endQuote <= startQuote + 1) {
        cout << "invalid command\n";
        return {};
    }

    string instructionBlock = command.substr(startQuote + 1, endQuote - startQuote - 1);
    vector<string> instructions;
    istringstream iss(instructionBlock);
    string instruction;

    while (getline(iss, instruction, ';')) {
        // Trim leading and trailing whitespace
        instruction.erase(0, instruction.find_first_not_of(" \t\n\r"));
        instruction.erase(instruction.find_last_not_of(" \t\n\r") + 1);

        if (!instruction.empty()) {
            instructions.push_back(instruction);
        }
    }

    if (instructions.empty() || instructions.size() > 50) {
        cout << "invalid command\n"; // i think this should be throwed??? im not sure kindly double check tysm
        return {};
    }

    return instructions;
}

int main(){
    srand(static_cast<unsigned int>(time(0)));
    string command;
    bool initialized = false;
    Config config;
    shared_ptr<Scheduler> scheduler;

    header();
    do {
        string screenName = "";
        cout << "Enter command: ";
        getline(cin, command);
        consoleStrings.append("Enter Command: " + command + "\n");

        // initialize first before giving access to other commands
        if (!initialized) {
            if (command == "initialize") {
                config = loadConfig();

                thread cycleThread(cpuCycleThread, config.delay);
                cycleThread.detach();

                Scheduler::Mode mode;
                if (config.schedulerMode == "rr") {
                    mode = Scheduler::Mode::RR;
                }
                else if (config.schedulerMode == "fcfs") {
                    mode = Scheduler::Mode::FCFS;
                }
                else {
                    cout << "Invalid scheduler mode!\n";
                    return 1;
                }

                //shared_ptr<FlatMemoryAllocator> allocator = make_shared<FlatMemoryAllocator>(config.maxMem);
                pagingallocator = make_shared<PagingAllocator>(config.maxMem, config.memFrame);
                scheduler = std::make_shared<Scheduler>(mode, config.quantum, config.numCPU, config.delay, pagingallocator, config.minIns, config.maxIns);
                scheduler->run();

                string initialize = "\n"
                    "\x1B[32m\x1B[1mSuccessfully initialized system\x1B[22m\x1B[0m\n"
                    "CPUs: " + std::to_string(config.numCPU) + "\n" +
                    "Scheduler: " + config.schedulerMode + "\n" +
                    "Quantum: " + std::to_string(config.quantum) + "\n" +
                    "Batch Freq: " + std::to_string(config.batchFreq) + "\n" +
                    "Min Instructions: " + std::to_string(config.minIns) + "\n" +
                    "Max Instructions: " + std::to_string(config.maxIns) + "\n" +
                    "Delay: " + std::to_string(config.delay) + "\n" +
                    "Max Overall Memory: " + to_string(config.maxMem) + "\n" +
					"Max Memory per Frame: " + to_string(config.memFrame) + "\n" +
					"Min Memory per Process: " + to_string(config.minMemProc) + "\n" +
					"Max Memory per Process: " + to_string(config.maxMemProc) + "\n";

                cout << initialize;
                consoleStrings.append(initialize);

                initialized = true;
            }
            else if (command == "exit") {
                exit(0);
            }
            else {
                cout << "\x1B[31m\x1B[1mError:\x1B[0m System not initialized. Please run 'initialize' first or 'exit' to quit.\n";
				consoleStrings.append("\x1B[31m\x1B[1mError:\x1B[0m System not initialized. Please run 'initialize' first or 'exit' to quit.\n");
            }
            continue;
        }

        if (command == "clear") {
            clear();
        } else if (command == "help") {
            help();
        } else if (command == "initialize") {
            cout << "\x1B[31m\x1B[1mError:\x1B[0m System already initialized.\n";
			consoleStrings.append("\x1B[31m\x1B[1mError:\x1B[0m System already initialized.\n");
        } else if (command == "scheduler-start") {
            cout << "\x1B[32m\x1B[1mscheduler-start\x1B[22m\x1B[0m command recognized. Generating Processes.\n";
			thread schedulerThread(scheduler_start, config, ref(*scheduler));
			schedulerThread.detach(); // Detach the thread to run scheduler_start in the background
        } else if (command == "scheduler-stop") {
            if (processGeneration == true) {
                processGeneration = false;
                cout << "\x1B[32m\x1B[1mscheduler-stop\x1B[22m\x1B[0m command recognized. Stopping generation.\n";
				consoleStrings.append("\x1B[32m\x1B[1mscheduler-stop\x1B[22m\x1B[0m command recognized. Stopping generation.\n");
            }
            else {
                cout << "\x1B[31m\x1B[1mError:\x1B[0m scheduler-start is not initialized/has stopped already.\n\n";
				consoleStrings.append("\x1B[31m\x1B[1mError:\x1B[0m scheduler-start is not initialized/has stopped already.\n\n");
            }
            
            //scheduler_stop();
        } else if (command == "report-util") {
            report_util(config, *scheduler);
        } else if (command == "exit") {
            exit(0); 
        } else if (command.rfind("screen -s", 0) == 0) {

            string rawScreenName = getScreenName(command); // get the process name after -s

            if (rawScreenName.empty()) { // makes sure theres a proper screen name
                cout << "\x1B[31m\x1B[1mError:\x1B[0m Screen name cannot be empty.\n";
				consoleStrings.append("\x1B[31m\x1B[1mError:\x1B[0m Screen name cannot be empty.\n");
            }
            else {
                screenName = rawScreenName;
                if (inScreenMap(screenName) == false) { // ensures screen name doesn't exist yet
                    //uint16_t memoryRequired = generateMem(config);
                    uint16_t processMemory = isValidMemory(command, config.maxMem); // check if the memory size for the process is valid
                    if (processMemory == 0) continue;
                    shared_ptr<Process> process = make_shared<Process>(globalPID, screenName, config.delay, processMemory, config.memFrame, config.maxMem, config.quantum, pagingallocator);
					//process->generateCommands(config.minIns, config.maxIns, 0);
					scheduler->addProcess(process); // add the process to the scheduler
                    
                    Console temp(process);
                    shared_ptr<Console> consolePtr = make_shared<Console>(temp);
                    screens.insert({ screenName, consolePtr });
					//screenInterface(screenName); // open the screen interface
                    globalPID++;
                }
                else {
                    cout << "\x1B[31m\x1B[1mError:\x1B[0m Screen name already exist/has finished executing!\n";
					consoleStrings.append("\x1B[31m\x1B[1mError:\x1B[0m Screen name already exist/has finished executing!\n");
                }
            }
        } else if (command.rfind("screen -c", 0) == 0) {
            string rawScreenName = getScreenName(command); // get the process name after -s

            if (rawScreenName.empty()) { // makes sure theres a proper screen name
                cout << "\x1B[31m\x1B[1mError:\x1B[0m Screen name cannot be empty.\n";
                consoleStrings.append("\x1B[31m\x1B[1mError:\x1B[0m Screen name cannot be empty.\n");
            }
            else {
                screenName = rawScreenName;
                if (inScreenMap(screenName) == false) { // ensures screen name doesn't exist yet
                    uint16_t processMemory = isValidMemory(command, config.maxMem); // check if the memory size for the process is valid
                    if (processMemory == 0) continue;

                    // process intructions
                    vector<string> instructions = parseInstructionsFromCommand(command);

                    // UNCOMMENT FOR VISUALIZATION
                    /*for (const string& instr : instructions) {
                        cout << instr << endl;
                    }*/

                    //shared_ptr<Process> process = make_shared<Process>(globalPID, screenName, config.delay, processMemory);
                    //process->initializeCommands(instructions);
                    //scheduler->addProcess(process); // add the process to the scheduler

                    //Console temp(process);
                    //shared_ptr<Console> consolePtr = make_shared<Console>(temp);
                    //screens.insert({ screenName, consolePtr });
                    ////screenInterface(screenName); // open the screen interface
                    //globalPID++;
                }
                else {
                    cout << "\x1B[31m\x1B[1mError:\x1B[0m Screen name already exist/has finished executing!\n";
                    consoleStrings.append("\x1B[31m\x1B[1mError:\x1B[0m Screen name already exist/has finished executing!\n");
                }
            }
        } else if (command.rfind("screen -r", 0) == 0){
            string rawScreenName = getScreenName(command);
            
            if (rawScreenName.empty()) { // makes sure theres a proper screen name
                cout << "\x1B[31m\x1B[1mError:\x1B[0m Screen name cannot be empty.\n";
				consoleStrings.append("\x1B[31m\x1B[1mError:\x1B[0m Screen name cannot be empty.\n");
            }
            else {
                screenName = rawScreenName;
                if (inScreenMap(screenName) == true) { // ensures screen name already exists
                    if (screens[screenName]->getProcess()->isFinished()) { // if the process is finished, it will not be able to run commands
                        cout << "\x1B[31m\x1B[1mError:\x1B[0m Process " << screens[screenName]->getProcess()->getName() << " not found.\n\n";
                        consoleStrings.append("\x1B[31m\x1B[1mError:\x1B[0m Process " + screens[screenName]->getProcess()->getName() + " not found.\n\n");
                    }
                    else {
                        screenInterface(screenName);
                    }
                }
                else {
                    cout << "\x1B[31m\x1B[1mError:\x1B[0m Screen name doesn't exist! Use 'screen -s <process name>' to create the screen.\n";
					consoleStrings.append("\x1B[31m\x1B[1mError:\x1B[0m Screen name doesn't exist! Use 'screen -s <process name>' to create the screen.\n");
                }
            }
        } else if (command == "screen -ls"){
            if (screens.empty()) {
                cout << "No screens available.\n";
				consoleStrings.append("No screens available.\n");
            } else {

                saveLs(ref(*scheduler), config);
            }
        }
        else if (command == "process-smi") {
			processSmi(ref(*scheduler), config); // call process smi
            pagingallocator->visualizeMemory();
        }
        else {
            cout << "\x1B[31m\x1B[1mUnknown command:\x1B[22m " << command << "\x1B[0m\n";
			consoleStrings.append("\x1B[31m\x1B[1mUnknown command:\x1B[22m " + command + "\x1B[0m\n");
        }
    } while (true);
   
    return 0;
}

/* TO DO LIST
    - [DONE] (not sure if need) empty screen name (from "screen -s  ")
    - [DONE] screen -s and -r does the same thing (make sure -s checks it doesnt exist and -r checks it does)
    - (optional for now) main menu text doesnt get saved
*/

/*
    1. Loading from config
    2. Sleep(X) for[instructions] command representations
    3. Thread to run processes (CPUs)
    - Based on how many is declared, create a thread, this
      thread will run the process assigned to it
    4. Thread for scheduling
    - also a scheduler thread that will run the scheduler
    - this will run the scheduler_start and scheduler_stop commands
    - make dummy processes that will arrive every X Cpu ticks
    -
    5. Report - util
    6. Detailed process - smi and screen - ls
*/

/*
    // FOR TESTING
    auto p1 = make_shared<Process>(1, "Process 1");
    auto p2 = make_shared<Process>(2, "Process 2");
    auto p3 = make_shared<Process>(3, "Process 3");
    auto p4 = make_shared<Process>(4, "Process 4");

    // Add Print Commands
    p1->addCommand(make_shared<SleepCommand>(p1, 10));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 1"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 2"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 3"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 4"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 5"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 6"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 7"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 8"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 9"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 10"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 11"));
    p1->addCommand(make_shared<PrintCommand>(p1, "Hello from P1 - Step 12"));
    p2->addCommand(make_shared<SleepCommand>(p2, 25));
    p2->addCommand(make_shared<PrintCommand>(p2, "Hello from P2 - Step 1"));
    p2->addCommand(make_shared<PrintCommand>(p2, "Hello from P2 - Step 2"));
    p2->addCommand(make_shared<PrintCommand>(p2, "Hello from P2 - Step 3"));
    p2->addCommand(make_shared<PrintCommand>(p2, "Hello from P2 - Step 4"));
    p2->addCommand(make_shared<PrintCommand>(p2, "Hello from P2 - Step 5"));
    p2->addCommand(make_shared<PrintCommand>(p2, "Hello from P2 - Step 6"));
    p3->addCommand(make_shared<SleepCommand>(p3, 30));
    p3->addCommand(make_shared<PrintCommand>(p3, "Hello from P3 - Step 1"));
    p3->addCommand(make_shared<PrintCommand>(p3, "Hello from P3 - Step 2"));
    p4->addCommand(make_shared<SleepCommand>(p4, 20));
    p4->addCommand(make_shared<PrintCommand>(p4, "Hello from P4 - Step 1"));
    p4->addCommand(make_shared<PrintCommand>(p4, "Hello from P4 - Step 2"));
    p4->addCommand(make_shared<PrintCommand>(p4, "Hello from P4 - Step 3"));

    scheduler.addProcess(p1);
    scheduler.addProcess(p2);
    scheduler.addProcess(p3);
    scheduler.addProcess(p4);

    shared_ptr<Process> process = make_shared<Process>(globalPID, "screenName");
    Console temp(process);
    shared_ptr<Console> consolePtr = make_shared<Console>(temp);
    screens.insert({ "screenName", consolePtr });
*/