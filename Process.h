#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <ctime>

using namespace std;

class Command;
class PagingAllocator;

class Process : public enable_shared_from_this<Process>
{
	public:
		enum ProcessState {
			READY,
			RUNNING,
			WAITING,
			FINISHED
		};

		Process(int pid, string name, uint32_t delay, uint16_t memoryRequired, uint16_t memFrame, uint16_t maxMem, uint32_t quantum, shared_ptr<PagingAllocator> allocator);
		void addCommand(shared_ptr<Command> command);
		void generateCommands(const uint32_t minIns, const uint32_t maxIns, int depth);
		vector<shared_ptr<Command>> generateRandomCommandList(int depth, int repeats, int& instructionBudget);
		void executeCommand();
		void moveToNextLine();
		void setCpuCoreID(int coreID);
		void writeLogsToFile(const string& filename) const;

		// In Process.h (add this member)
		

		bool isFinished() const;

		bool getIsProcessError() const;

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
		uint16_t getMemReq() const;
		uint16_t getNumPages() const;
		string saveLogs();
		void setAllocatedMemory(void* memory);
		void* getAllocatedMemory() { return allocatedMemory; }
		void initializeCommands(const vector<string>& instructions);
		void setAllocatedFrames(const vector<size_t>& frames, bool deallocate);
		void allocateVariable(const string& name, uint16_t value);
		void editVariable(const string& name, uint16_t value);
		void writeToMemory(uint16_t frameIndex, uint16_t address, uint16_t value);
		void readMemory(uint16_t frameIndex, uint16_t address, const string& varName);
		const vector<size_t>& getAllocatedFrames() const { return allocatedFrames; }
		vector<vector<bool>> getProcessMemory() const { return processMemory; }
		vector<vector<int>> getProcessMemoryRead() const { return processMemoryRead; }
		

		vector<shared_ptr<Command>> Process::getCommandList() const;

		// for sleep command
		void setSleepTicks(int ticks) { sleepTicks = ticks; }
		int getSleepTicks() const { return sleepTicks; }
		void terminateProcess();

		void decrementSleepTick() { if (sleepTicks > 0) --sleepTicks; }
		bool isSleeping() const { return sleeping; }
		void setSleeping(bool value) { sleeping = value; }
		void visualizeProcessMemory() const;

		int countNonForInstructions(const vector<shared_ptr<Command>>& cmds) const;
		bool getIsTerminated() const { return isTerminated; }
		void setHasCommands(bool value) { hasCommands = value; }
		bool getHasCommands() const { return hasCommands; }
		void visualizeProcessContents() const;
		int getTimestep() const { return timestep; }
		void setTimestep(int step) { timestep = step; }
		uint16_t getQuantum() { return quantum; }

		

	
	private:
		void* allocatedMemory;
		int pid; // Memory allocator for this process
		int variableCounter = 0; // Counter for variable names
		uint16_t memFrame; // Memory per frame
		string name;
		uint16_t maxMem;
		uint32_t quantum;
		bool isTerminated = false;
		vector<shared_ptr<Command>> commandList;
		vector<string> logLines;
		struct VariableInfo {
			uint16_t value;
			size_t address;
		};
		unordered_map<string, uint16_t> symbolTable;
		unordered_map<string, uint16_t> memoryNameTableFrame; //name of the variable and its frame
		unordered_map<string, uint16_t> memoryNameTable; //name of the variable and its address in the frame
		//void* allocatedMemory;
		vector<size_t> allocatedFrames; // this contains the frame index
		vector<vector<bool>> processMemory; // for read and write; not yet used GO PRINCE WOO
		vector<vector<int>> processMemoryRead; // for read operations
		bool hasCommands = false;
		int timestep = 0;
		bool isProcessError = false;

		uint16_t memoryRequired;
		int commandCounter = 0;
		int cpuCoreID = -1; // -1 means not assigned to any core	
		ProcessState currentState;

		int sleepTicks = 0;
		bool sleeping = false;
		int totalNumCommands = -1;
		uint32_t delay;
		
		shared_ptr<PagingAllocator> pagingallocator;
};