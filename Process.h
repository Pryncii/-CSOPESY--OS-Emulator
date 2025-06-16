#pragma once
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class Process
{
	public:
		enum ProcessState {
			READY,
			RUNNING,
			WAITING,
			FINISHED
		};

		Process(int pid, string name);
		void addCommand(string command);
		void executeCommand();
		void moveToNextLine();

		bool isFinished() const;
		int getPid() const;
	
	private:
		int pid;
		string name;
		vector<string> commandList;
		unordered_map<string, uint16_t> symbolTable;

		int commandCounter;
		int cpuCoreID = -1; // -1 means not assigned to any core	
		ProcessState currentState;


};

