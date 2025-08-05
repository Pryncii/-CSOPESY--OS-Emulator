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
		void savePageIndicesToFile(const std::string& filename) const;

		// In Process.h (add this member)
		

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
		uint16_t getMemReq() const;
		uint16_t getNumPages() const;
		string saveLogs();
		void setAllocatedMemory(void* memory);
		void* getAllocatedMemory() { return allocatedMemory; }
		void addToAllocatedFrames(size_t value);
		void initializeCommands(const vector<string>& instructions);
		void addToPageIndices(size_t value);
		void setPages(bool deallocate);
		void allocateVariable(const string& name, uint16_t value);
		void editVariable(const string& name, uint16_t value);
		void writeToMemory(uint16_t frameIndex, uint16_t address, uint16_t value);
		void readMemory(uint16_t frameIndex, uint16_t address, const string& varName);
		const vector<size_t>& getAllocatedFrames() const { return allocatedFrames; }
		vector<vector<bool>> getProcessMemory() const { return processMemory; }
		vector<vector<int>> getProcessMemoryRead() const { return processMemoryRead; }
		bool getIsProcessError() const;
		void removeFromAllocatedFrames(size_t frame);
		

		vector<shared_ptr<Command>> Process::getCommandList() const;
		int getCommandCounter() const { return commandCounter; }	
		// for sleep command
		void setSleepTicks(int ticks) { sleepTicks = ticks; }
		int getSleepTicks() const { return sleepTicks; }
		void terminateProcess();

		void decrementSleepTick() { if (sleepTicks > 0) --sleepTicks; }
		bool isSleeping() const { return sleeping; }
		void setSleeping(bool value) { sleeping = value; }
		void visualizeProcessMemory() const;
		void deletePageIndexFromFile(const std::string& filename, const std::string& processName, uint16_t pageToDelete) const;
		int countNonForInstructions(const vector<shared_ptr<Command>>& cmds) const;
		bool getIsTerminated() const { return isTerminated; }
		void setHasCommands(bool value) { hasCommands = value; }
		bool getHasCommands() const { return hasCommands; }
		void visualizeProcessContents() const;
		int getTimestep() const { return timestep; }
		void setTimestep(int step) { timestep = step; }
		uint16_t getQuantum() { return quantum; }
		unordered_map<string, uint16_t> getMemoryNameTable() const { return memoryNameTable; }
		uint16_t getMemFrame() const { return memFrame; }
		vector<size_t> getPageIndices() const { return pageIndices; }
		shared_ptr<PagingAllocator> getPagingAllocator() const { return pagingallocator; }
	
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
		bool isProcessError = false;
		//void* allocatedMemory;

		/*
		- When the process is loaded in, all our page indices will be
		  in the backing store
		- We will only page in and allocate to the main memory if an
		  address is accessed that is not in the main memory
		- So for the backing store, we will need the process ID, page number,
		  and the contents of that page, probably the addresses of variable names(so if we add/subtract a variable,
		  we'll first check if the page indices containing the symbol table is in the process, if not,
		  call the backing store)
		

		*/
		
		vector<size_t> pageIndices; // this contains the address in the frame

	
		vector<size_t> allocatedFrames; // this contains the frame index
		vector<vector<bool>> processMemory; // for read and write; if true, memory spot is used
		vector<vector<int>> processMemoryRead; // for read operations; contains the value
		bool hasCommands = false;
		int timestep = 0;

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

/*
IDEA:
- when process gets inside CPU, throw everything to backing store first
- then when process's command needs something from an address: EX: process1 says ADD varA varB varC
- use ALLOCATEDFRAMES (contains phys mem frames); use that to search mem frames


READ varA 0x99


struct FrameEntry {
	bool valid;                 // Valid bit
	uint16_t processID;         // Owning process
	uint16_t pageNumber;        // Which page of the process
	vector<int> memoryContents; // Contents in this page (e.g. integers)
};


*/ 
