#include <iostream>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

struct commandinformation {
    std::string command;
    int noofcommand;
} commandinformation;

struct Command {
    bool is_back = false;
    int com_id = -1;
    std::string option;
    std::string para_a;
    std::string para_b;
    bool is_in_relocate = false;
    bool is_out_relocate = false;
    bool cover_or_add = false;
    std::string relocated_file;
    bool is_head = false;
    bool is_tail = false;
    const char* out;
    const char* in;
};

void printthetree(char* name, int root);
void tree(char* name, char* basepath, const int root);
void printthepath(char* name, int root);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " is_back com_id option para_a para_b is_in_relocate is_out_relocate cover_or_add relocated_file is_head is_tail out in" << std::endl;
        return 1;
    }

    Command com_block;

    com_block.is_back = std::stoi(argv[1]);
    com_block.com_id = std::stoi(argv[2]);
    com_block.option = argv[3];
    com_block.para_a = argv[4];
    com_block.para_b = argv[5];
    com_block.is_in_relocate = std::stoi(argv[6]);
    com_block.is_out_relocate = std::stoi(argv[7]);
    com_block.cover_or_add = std::stoi(argv[8]);
    com_block.relocated_file = argv[9];
    com_block.is_head = std::stoi(argv[10]);
    com_block.is_tail = std::stoi(argv[11]);
    com_block.out = argv[12];
    com_block.in = argv[13];

    if (com_block.para_a.empty()) {
        com_block.para_a = ".";
    }

    commandinformation.noofcommand = com_block.option.length();
    commandinformation.command = com_block.option;

    tree(const_cast<char*>(com_block.para_a.c_str()), const_cast<char*>(com_block.para_a.c_str()), 0);

    return 0;
}

bool findcommand(char p) {
    return commandinformation.command.find(p) != std::string::npos;
}

void tree(char* name, char* basepath, int root) {
    char path[1000];
    struct dirent* dp;
    if (findcommand('f'))
        printthepath(basepath, root);
    else
        printthetree(name, root);
    DIR* dir = opendir(basepath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && (dp->d_name[0] != '.' || findcommand('a'))) {

            strcpy(path, basepath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            if (findcommand('d') == 0 || dp->d_type != DT_REG)
                tree(dp->d_name, path, root + 1);
        }
    }

    closedir(dir);
}

void printthetree(char* name, int root) {
    int i;
    if (!findcommand('i')) {
        for (i = 0; i < root - 1; i++) {
            printf("   ");
        }
        if (root)
            printf("|__");
    }
    printf("%s\n", name);
}

void printthepath(char* name, int root) {
    int i;
    if (!findcommand('i')) {
        for (i = 0; i < root - 1; i++) {
            printf("   ");
        }
        if (root)
            printf("|__");
    }
    printf("%s\n", name);
}
