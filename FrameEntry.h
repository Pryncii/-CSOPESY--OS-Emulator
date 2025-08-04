#pragma once
// FrameEntry.h
#pragma once
#include <vector>
#include <cstdint>
using namespace std;

struct FrameEntry {
	bool valid;                 // Valid bit
	uint16_t processID;         // Owning process
	uint16_t pageNumber;        // Which page of the process
	vector<string> memoryContentsVarName; // Contents in this page (e.g. varName)
	vector<int> memoryContents; // Contents in this page (e.g. integers)
};
