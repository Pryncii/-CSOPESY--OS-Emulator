#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <ctime>

using namespace std;

class Command;

class Process : public enable_shared_from_this<Process>
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
		void generateCommands();
		void executeCommand();
		void moveToNextLine();
		void setCpuCoreID(int coreID);
		void writeLogsToFile(const string& filename) const;

		bool isFinished() const;
		int getPID() const;
		int getCpuCoreID() const;
		string getTime() const;
		string timestamp;
		string getName() const;
		int getCurLine() const;
		int getTotalLines() const;
		unordered_map<string, uint16_t> getSymbolTable() const;
		void addSymbol(const string& symbol, uint16_t value);
		uint16_t getSymbolValue(const string& symbol);
	
	private:
		int pid;
		string name;
		vector<shared_ptr<Command>> commandList;
		unordered_map<string, uint16_t> symbolTable;

		int commandCounter = 0;
		int cpuCoreID = -1; // -1 means not assigned to any core	
		ProcessState currentState;


};