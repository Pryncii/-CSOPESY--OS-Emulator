#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Command.h"



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
		void addCommand(shared_ptr<Command> command);
		void executeCommand();
		void moveToNextLine();

		bool isFinished() const;
		int getPid() const;
		unordered_map<string, uint16_t> getSymbolTable() const;
		void addSymbol(const string& symbol, uint16_t value);
		uint16_t getSymbolValue(const string& symbol);
	
	private:
		int pid;
		string name;
		vector<shared_ptr<Command>> commandList;
		unordered_map<string, uint16_t> symbolTable;

		int commandCounter;
		int cpuCoreID = -1; // -1 means not assigned to any core	
		ProcessState currentState;


};

/*


*/
