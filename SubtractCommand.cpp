#include "SubtractCommand.h"

using namespace std;

SubtractCommand::SubtractCommand(shared_ptr<Process> process, const string& var1, uint16_t var2, uint16_t var3)
    : Command(process, CommandType::SUBTRACT) {
    this->var1 = var1;
    this->var2Num = var2;
    this->var3Num = var3;
    this->var2IsNumber = true;
    this->var3IsNumber = true;
}

SubtractCommand::SubtractCommand(shared_ptr<Process> process, const string& var1, const string& var2, const string& var3)
    : Command(process, CommandType::SUBTRACT) {
    this->var1 = var1;
    this->var2Str = var2;
    this->var3Str = var3;
    this->var2IsNumber = false;
    this->var3IsNumber = false;
}

SubtractCommand::SubtractCommand(shared_ptr<Process> process, const string& var1, uint16_t var2, const string& var3)
    : Command(process, CommandType::SUBTRACT) {
    this->var1 = var1;
    this->var2Num = var2;
    this->var3Str = var3;
    this->var2IsNumber = true;
    this->var3IsNumber = false;
}

SubtractCommand::SubtractCommand(shared_ptr<Process> process, const string& var1, const string& var2, uint16_t var3)
    : Command(process, CommandType::SUBTRACT) {
    this->var1 = var1;
    this->var2Str = var2;
    this->var3Num = var3;
    this->var2IsNumber = false;
    this->var3IsNumber = true;
}

void SubtractCommand::execute() {
    uint16_t result = 0;
    if (var2IsNumber && var3IsNumber) {
        result = var2Num - var3Num;
        process->addSymbol(var1, result);
    }
    else if (var2IsNumber && !var3IsNumber) {
        result = var2Num - process->getSymbolValue(var3Str);
        process->addSymbol(var1, result);
    }
    else if (!var2IsNumber && var3IsNumber) {
        result = process->getSymbolValue(var2Str) - var3Num;
        process->addSymbol(var1, result);
    }
    else {
        result = process->getSymbolValue(var2Str) - process->getSymbolValue(var3Str);
        process->addSymbol(var1, result);
    }
}