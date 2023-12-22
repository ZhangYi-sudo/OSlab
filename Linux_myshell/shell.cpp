#include"shell.h"
string processString(const string& input) {
    string result;
    bool isPreviousCharSpace = true;  // Flag to track if previous character is a space

    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '>') {
            if (input[i + 1] == '>') {
                if (!isPreviousCharSpace) {
                    result += ' ';
                }
                result += ">> ";
                isPreviousCharSpace = true;
                ++i;  // Skip the next character since it's part of ">>"
            }
            else if (input[i + 1] == '<') {
                if (!isPreviousCharSpace) {
                    result += ' ';
                }
                result += "> ";
                isPreviousCharSpace = true;
            }
            else {
                result += "> ";
                isPreviousCharSpace = true;
            }
        }
        else if (input[i] == '<') {
            if (input[i + 1] == '<') {
                if (!isPreviousCharSpace) {
                    result += ' ';
                }
                result += "<< ";
                isPreviousCharSpace = true;
                ++i;  // Skip the next character since it's part of "<<"
            }
            else {
                result += "< ";
                isPreviousCharSpace = true;
            }
        }
        else if (input[i] == '-') {
            result += " -";
            isPreviousCharSpace = false;
        }
        else {
            result += input[i];
            isPreviousCharSpace = false;
        }
    }

    return result;
}

bool isBackRun(string& input) {
    // Remove trailing spaces
    auto length = input.length();
    int i = length - 1;
    while (i >= 0) {
        if (input[i] == ' ' || input[i] == '\0') {
            i--;
        }
        else {
            break;
        }
    }
    input = input.substr(0, i + 1);
    length = input.length();
    if (input[length - 1] == '&') {
        input.pop_back();
        return true;
    }
    return false;
}
int detectValidCommand(string input) {
    const string com_name_list[] = {
    FUNCTION_LIST
    };

    int i = COM_NUM - 1;
    while (i >= 0) {
        if (com_name_list[i] == input) {
            return i;
        }
        i--;
    }
    return -1;
}

int commmadSplit(const char* com, std::vector<Command>& com_list, int com_num) {
    int command_count = 0, i = 0;
    std::vector<std::string> com_vec(com_num);

    while (true) {
        if (com[i] == '\0') {
            com_vec[command_count].push_back('\0');
            command_count++;
            i++;
            break;
        }
        if (com[i] == '|') {
            com_vec[command_count].push_back('\0');
            command_count++;
            i++;
            continue;
        }
        com_vec[command_count].push_back(com[i]);
        i++;
    }

    if (com_num != command_count) {
        std::cout << "Inner Error: Mismatched Command Counts!\n";
        return -1;
    }

    if (com_list.empty()) {
        std::cout << "Inner Error: Empty Command List!\n";
        return -1;
    }
    else {
        for (int i = 0; i < command_count; i++) {
            if (i == command_count - 1) {
                com_list[i].is_tail = true;
            }
            if (i == 0) {
                com_list[i].is_head = true;
            }
            auto this_command = com_vec[i];
            this_command = processString(this_command);
            com_list[i].is_back = isBackRun(this_command);
            std::istringstream iss(this_command);
            std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                std::istream_iterator<std::string>());

            com_list[i].is_in_relocate = (this_command.find("< ") != std::string::npos) || (this_command.find("<<") != std::string::npos);
            com_list[i].is_out_relocate = (this_command.find("> ") != std::string::npos) || (this_command.find(">>") != std::string::npos);
            com_list[i].cover_or_add = (this_command.find(" > ") != std::string::npos);

            int com_id = detectValidCommand(tokens[0]);
            if (com_id == -1) {
                std::cout << "Error: Unknown Command:" << tokens[0] << "\n";
                return -1;
            }
            else {
                com_list[i].com_id = com_id;
            }

            for (size_t j = 1; j < tokens.size(); ++j) {
                if (tokens[j][0] == '-') {
                    com_list[i].option.append(tokens[j].substr(1));
                }
            }

            int count = 0;
            for (size_t j = 1; j < tokens.size(); ++j) {
                if (count == 0 && tokens[j][0] != '-' && tokens[j] != ">>" && tokens[j] != "<<" && tokens[j] != ">" && tokens[j] != "<") {
                    com_list[i].para_a = tokens[j];
                    count++;
                    continue;
                }
                if (count == 1 && tokens[j][0] != '-' && tokens[j] != ">>" && tokens[j] != "<<" && tokens[j] != ">" && tokens[j] != "<") {
                    com_list[i].para_b = tokens[j];
                    count++;
                    continue;
                }
                if (count == 2 && tokens[j][0] != '-' && tokens[j] != ">>" && tokens[j] != "<<" && tokens[j] != ">" && tokens[j] != "<") {
                    com_list[i].relocated_file = tokens[j];
                    count++;
                    continue;
                }
            }

            if (com_list[i].is_in_relocate || com_list[i].is_out_relocate) {
                if (com_list[i].relocated_file.empty()) {
                    if (!com_list[i].para_b.empty()) {
                        com_list[i].relocated_file = com_list[i].para_b;
                        com_list[i].para_b.clear();
                    }
                    else {
                        if (!com_list[i].para_a.empty()) {
                            com_list[i].relocated_file = com_list[i].para_a;
                            com_list[i].para_a.clear();
                        }
                    }
                }
            }
        }
    }

    return command_count;
}

const char* run(struct Command* com_block, int rank, int counts, int& result) {
    if (com_block->com_id == -1) {
        return NULL;
    }
    result = fv[com_block->com_id](com_block);
    if (rank >= 0 && rank != counts - 1) {
        return com_block->out;
    }
    else {
        return NULL;
    }
}

// Ϊ��������ָ� Command �����װΪ�������� 
void constructCommandArgs(Command* com_block, vector<const char*>& args) {
    // ��ȡ��ǰĿ¼
    char current_directory[4096];
    if (getcwd(current_directory, sizeof(current_directory)) == NULL) {
        perror("getcwd");
        exit(1);
    }

    // �����ļ�·����"bin" Ŀ¼�ڵ�ǰĿ¼�£����ļ���"bin"Ŀ¼��
    char filename[4096]; // ���ӻ�������С
    int len = snprintf(filename, sizeof(filename), "%s/bin/%s", current_directory, com_name_list[com_block->com_id].c_str());
    if (len >= sizeof(filename)) {
        cerr << "Error: Buffer too small for filename." << endl;
        exit(1);
    }

    args = {
        filename,
        to_string(com_block->is_back).c_str(),
        to_string(com_block->com_id).c_str(),
        com_block->option.c_str(),
        com_block->para_a.c_str(),
        com_block->para_b.c_str(),
        to_string(com_block->is_in_relocate).c_str(),
        to_string(com_block->is_out_relocate).c_str(),
        to_string(com_block->cover_or_add).c_str(),
        com_block->relocated_file.c_str(),
        to_string(com_block->is_head).c_str(),
        to_string(com_block->is_tail).c_str(),
        com_block->out,
        com_block->in
    };

    args.push_back(nullptr);
}

