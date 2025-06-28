#pragma once
#include "ForLoopCommand.h"
#include <string>
#include <iostream>
#include <vector>
#include "Command.h"
using namespace std;

class ForLoopCommand : public Command {
public:
	ForLoopCommand(shared_ptr<Process> process, vector<shared_ptr<Command>> commandList, int repeats);
	void execute() override;
	void nested(const vector<shared_ptr<Command>>& commands, vector<shared_ptr<Command>>& flatList, int repeats, int depth);

	string toString() const override {
		return "[FOR]";
	}

	int getRepeats() const {
		return repeats;
	}

	vector<shared_ptr<Command>> getCommandList() const {
		return commandList;
	}

private:
	shared_ptr<Process> process;
	vector<shared_ptr<Command>> commandList;
	int repeats;


};

