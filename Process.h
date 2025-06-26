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
		void generateCommands(const uint32_t minIns, const uint32_t maxIns, int depth);
		vector<shared_ptr<Command>> generateRandomCommandList(const uint32_t minIns, const uint32_t maxIns, int depth);
		void executeCommand();
		void moveToNextLine();
		void setCpuCoreID(int coreID);
		void writeLogsToFile(const string& filename) const;

		bool isFinished() const;
		int getPID() const;
		int getCpuCoreID() const;
		void printLogs() const;
		string getTime() const;
		string timestamp;
		string getName() const;
		int getCurLine() const;
		int getTotalLines() const;
		void addLogLine(const string& line);
		unordered_map<string, uint16_t> getSymbolTable() const;
		void addSymbol(const string& symbol, uint16_t value);
		uint16_t getSymbolValue(const string& symbol);
		string saveLogs();

		// for sleep command
		void setSleepTicks(int ticks) { sleepTicks = ticks; }
		int getSleepTicks() const { return sleepTicks; }

		void decrementSleepTick() { if (sleepTicks > 0) --sleepTicks; }
		bool isSleeping() const { return sleeping; }
		void setSleeping(bool value) { sleeping = value; }

	
	private:
		int pid;
		string name;
		vector<shared_ptr<Command>> commandList;
		vector<string> logLines;
		unordered_map<string, uint16_t> symbolTable;

		int commandCounter = 0;
		int cpuCoreID = -1; // -1 means not assigned to any core	
		ProcessState currentState;

		int sleepTicks = 0;
		bool sleeping = false;

		

};