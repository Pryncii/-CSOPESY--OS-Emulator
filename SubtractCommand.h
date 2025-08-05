#pragma once

#include "Command.h"
#include <string>
#include <iostream>

class SubtractCommand : public Command {
public:
    SubtractCommand(std::shared_ptr<Process> process, const std::string& var1, uint16_t var2, uint16_t var3);
    SubtractCommand(std::shared_ptr<Process> process, const std::string& var1, const std::string& var2, const std::string& var3);
    SubtractCommand(std::shared_ptr<Process> process, const std::string& var1, uint16_t var2, const std::string& var3);
    SubtractCommand(std::shared_ptr<Process> process, const std::string& var1, const std::string& var2, uint16_t var3);
    void execute() override;
    void pageIn(const string& varName);

    string toString() const override {
        return "[SUBTRACT]";
    }

private:
    std::string var1;
    std::string var2Str;
    std::string var3Str;
    bool var2IsNumber = false;
    bool var3IsNumber = false;
    uint16_t var2Num = 0;
    uint16_t var3Num = 0;
};