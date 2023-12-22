#ifndef SHELL_H
#define SHELL_H

#include "util.h"
#include "command.h"

#define MAX_COMMAND_SIZE 2048

string processString(const string& input);
bool isBackRun(string& input);
int commmadSplit(const char* com, std::vector<Command>& com_list, int com_num);
int detectValidCommand(string input);
const char* run(struct Command* com_block, int rank, int counts, int& result);
void constructCommandArgs(Command* com_block, vector<const char*>& args);
const string com_name_list[] = {
    FUNCTION_LIST
};
#endif