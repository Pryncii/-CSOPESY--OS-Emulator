#include "ForLoopCommand.h"
#include "Command.h"
#include <string>
#include <iostream>
#include <mutex>
#include <random>

using namespace std;
extern mutex coutMutex;

ForLoopCommand::ForLoopCommand(shared_ptr<Process> process, vector<shared_ptr<Command>> commandList, int repeats) : Command(process, FOR) {
    this->process = process;
    this->commandList = commandList;
    this->repeats = repeats;
}

void ForLoopCommand::nested(const vector<shared_ptr<Command>>& commands, vector<shared_ptr<Command>>& flatList, int repeats, int depth) {
    if (depth > 3) {
        throw std::runtime_error("Exceeded maximum FOR loop nesting depth (3)");
    }
    for (int i = 0; i < repeats; ++i) {
        for (const auto& cmd : commands) {
            auto forCmd = dynamic_pointer_cast<ForLoopCommand>(cmd);
            if (forCmd) {
                nested(forCmd->commandList, flatList, forCmd->repeats, depth + 1);
            }
            else {
                flatList.push_back(cmd);
            }
        }
    }
}

void ForLoopCommand::execute() {
    //lock_guard<mutex> lock(coutMutex);
    vector<shared_ptr<Command>> flatList;
    nested(commandList, flatList, repeats, 1);
    for (auto& cmd : flatList) {
        cmd->execute();
    }
}