#include <iostream>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <string>
#include <unistd.h>
#include <cstdio>


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

bool is_directory(const std::string& path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0) {
        return false;
    }
    return S_ISDIR(path_stat.st_mode);
}

// 判断源和目标路径尾缀是否相同 
bool is_same(std::string para_a, std::string para_b) {
    // 找到最后一个斜杠的位置
    size_t last_slash_a = para_a.find_last_of('/');
    size_t last_slash_b = para_b.find_last_of('/');

    // 提取最后一个斜杠之后的子串
    std::string filename_a = para_a.substr(last_slash_a + 1);
    std::string filename_b = para_b.substr(last_slash_b + 1);

    return (filename_a == filename_b);
}

int mv(Command* com_block) {
    if (com_block->para_a.empty() || com_block->para_b.empty()) {
        std::cerr << "mv: missing file operand" << std::endl;
        return 1;
    }

    std::string source_path = com_block->para_a;
    std::string destination_path = com_block->para_b;

    if (access(source_path.c_str(), F_OK) == -1) {
        std::cerr << "mv: cannot stat '" << source_path << "': No such file or directory" << std::endl;
        return 1;
    }

    bool source_is_directory = is_directory(source_path);
    bool destination_is_directory = is_directory(destination_path);

    std::string target_path;
    // 目标是目录
    if (destination_is_directory) {
        // 根据来源命名路径，是到目录还是到特定的文件
        if (source_is_directory) {
            target_path = destination_path + "/" + source_path.substr(source_path.find_last_of("/\\") + 1);
        }
        else {
            target_path = destination_path + "/" + source_path;
        }
        // 目标是文件
    }
    else {
        target_path = destination_path;
    }

    bool same = is_same(com_block->para_a, com_block->para_b);
    // 参数为 i
    if (com_block->option == "i") {
        if (same) {
            char userInput;
            bool validInput = false;

            do {
                std::cout << "File already exists. Do you want to overwrite? (y/n): ";
                std::cin >> userInput;

                if (userInput == 'y' || userInput == 'Y') {
                    // overwrite the file
                    if (rename(source_path.c_str(), target_path.c_str()) != 0) {
                        std::cerr << "mv: cannot move '" << source_path << "' to '" << target_path << "': " << strerror(errno) << std::endl;
                        return 1;
                    }
                    std::cout << "File overwritten." << std::endl;
                    std::cout << "mv: '" << source_path << "' to '" << target_path << "'" << std::endl;
                    validInput = true;
                }
                else if (userInput == 'n' || userInput == 'N') {
                    // not to overwrite
                    std::cout << "Operation canceled." << std::endl;
                    validInput = true;
                }
                else {
                    // invalid option
                    std::cout << "Invalid input. Please enter 'y' or 'n'." << std::endl;
                }
            } while (!validInput);
        }
    }
    // 参数为 f
    else if (com_block->option == "f"){
        if (rename(source_path.c_str(), target_path.c_str()) != 0) {
            std::cerr << "mv: cannot move '" << source_path << "' to '" << target_path << "': " << strerror(errno) << std::endl;
            return 1;
        }
        std::cout << "mv: '" << source_path << "' to '" << target_path << "'" << std::endl;
    }
    // 参数为 n
    else if (com_block->option == "n") {
        if(!same){
            if (rename(source_path.c_str(), target_path.c_str()) != 0) {
                std::cerr << "mv: cannot move '" << source_path << "' to '" << target_path << "': " << strerror(errno) << std::endl;
                return 1;
            }
            std::cout << "mv: '" << source_path << "' to '" << target_path << "'" << std::endl;
        }
        else {
            std::cout << "A directory/file of the same name exists and is not overwritten!" << std::endl;
        }
    }
    // 参数为 b
    else if (com_block->option == "b") {
        if (!same) {
            if (rename(source_path.c_str(), target_path.c_str()) != 0) {
                std::cerr << "mv: cannot move '" << source_path << "' to '" << target_path << "': " << strerror(errno) << std::endl;
                return 1;
            }
            std::cout << "mv: '" << source_path << "' to '" << target_path << "'" << std::endl;
        }
        else {
            // 创建备份文件名
            std::size_t lastDot = target_path.find_last_of(".");
            std::string backup_suffix = "_back";
            std::string backup_path = target_path.substr(0, lastDot) + backup_suffix + target_path.substr(lastDot);

            // 尝试创建备份
            if (rename(target_path.c_str(), backup_path.c_str()) != 0) {
                std::cerr << "mv: cannot create backup for '" << target_path << "': " << strerror(errno) << std::endl;
                return 1;
            }

            // 执行覆盖操作
            if (rename(source_path.c_str(), target_path.c_str()) != 0) {
                std::cerr << "mv: cannot move '" << source_path << "' to '" << target_path << "': " << strerror(errno) << std::endl;
                return 1;
            }

            std::cout << "mv: '" << source_path << "' to '" << target_path << "', backup created at '" << backup_path << "'" << std::endl;
        }
    }
    // 无参数
    else{
        if (rename(source_path.c_str(), target_path.c_str()) != 0) {
            std::cerr << "mv: cannot move '" << source_path << "' to '" << target_path << "': " << strerror(errno) << std::endl;
            return 1;
        }
        std::cout << "mv: '" << source_path << "' to '" << target_path << "'" << std::endl;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " is_back com_id option para_a para_b is_in_relocate is_out_relocate cover_or_add relocated_file is_head is_tail out in" << std::endl;
        return 1;
    }

    Command cmd;

    // Assuming the correct number of arguments (13 in this case)
    cmd.is_back = std::stoi(argv[1]);
    cmd.com_id = std::stoi(argv[2]);
    cmd.option = argv[3];
    cmd.para_a = argv[4];
    cmd.para_b = argv[5];
    cmd.is_in_relocate = std::stoi(argv[6]);
    cmd.is_out_relocate = std::stoi(argv[7]);
    cmd.cover_or_add = std::stoi(argv[8]);
    cmd.relocated_file = argv[9];
    cmd.is_head = std::stoi(argv[10]);
    cmd.is_tail = std::stoi(argv[11]);
    cmd.out = argv[12];
    cmd.in = argv[13];

    int result = mv(&cmd);

    if (result == 0) {
        std::cout << "mv command executed successfully." << std::endl;
    }
    else {
        std::cerr << "mv command failed." << std::endl;
    }

    return 0;
}

