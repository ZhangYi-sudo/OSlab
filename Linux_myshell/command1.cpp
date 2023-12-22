#include "command.h"
#include "shell.h"
#include "util.h"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"
#define RED "\033[0;31m"
FunVector fv[] = {Redirection, help, pwd, mv,  ls, history, rm};

int Redirection(Command* com_block) {
    pid_t pid;
    int fd_in;
    int fd_out;

    switch (pid = fork()) {
    case -1: {
        cout << "Fail to fork a child process!" << endl;
        return 0;
    }
    case 0: {
        // ���������ض���
        if (com_block->is_in_relocate) {
            fd_in = open(com_block->relocated_file.c_str(), O_RDONLY, 0666);

            if (fd_in < 0) {
                perror("open");
                // ���������Ϣ;
                return -1;
            }

            ssize_t bytesRead;
            char redirectBuffer[1024];
            bytesRead = read(fd_in, redirectBuffer, sizeof(redirectBuffer));

            if (fd_in != STDIN_FILENO) {
                close(fd_in);
            }

            if (bytesRead < 0) {
                perror("read");
                // ���������Ϣ
                return -1;
            }

            redirectBuffer[bytesRead] = '\0';

            // �ָ��ַ���
            std::istringstream iss(redirectBuffer);
            std::string token;

            if (com_block->para_a.empty() && com_block->para_b.empty())
            {
                if (iss >> token) {

                    com_block->para_a = token;

                    if (iss >> token) {
                        com_block->para_b = token;
                    }
                    else {
                        // ���û�еڶ������ʣ����Խ� para_b ��Ϊ�� para_a ��ͬ
                        com_block->para_b = "";
                    }
                }
            }
            else
            {
                if (iss >> token) {
                    com_block->para_b = token;
                }
            }
        }

        // ��������ض���
        if (com_block->is_out_relocate) {
            if (com_block->cover_or_add) {
                fd_out = open(com_block->relocated_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666); // ����ģʽ
            }
            else if (!com_block->cover_or_add) {
                fd_out = open(com_block->relocated_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666); // ׷��ģʽ
            }

            if (fd_out < 0) {
                perror("open");
                // �������������Ϣ�����磺cout << "Error opening output file: " << strerror(errno) << endl;
                exit(1);
            }

            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("dup2");
                // ���������Ϣ
                return -1;
            }
        }

        int result = 0;
        run(com_block, 0, 1, result);
        if (com_block->is_out_relocate) {
            // �ر�
            if (fd_out != STDOUT_FILENO) {
                close(fd_out);
                return -1;
            }
        }
        return result;
    }

    default: {
        int status;
        waitpid(pid, &status, 0);       // �ȴ��ӽ��̷���
        if (WIFEXITED(status)) {
            int err = WEXITSTATUS(status);  // ��ȡ�ӽ��̵ķ�����

            if (err) {
                cout << "Error: " << strerror(err) << endl;
            }
        }
        else {
            cout << "Child process terminated abnormally." << endl;
        }
    }
    }
    return 1;
}


/// ��ӡ�ֽ���
void printDivider()
{
    cout << "**********************************************************************************************************************" << endl;
}
int help(Command *com_block)
{
    // ���ֻ�����롰help����չʾ����ȫò
    if (com_block->option.empty() && com_block->para_a.empty() && com_block->para_b.empty())
    {
        cout << "\033[0;32m" << std::left << std::setw(15) << "Command"
             << "\033[0;32m" << std::left << std::setw(65) << "Description"
             << "\033[0;32m" << std::left << std::setw(30) << "Query"
             << "\033[0m" << std::endl;
        cout << std::left << std::setw(15) << "help" << std::left << std::setw(65) << "Display all commands and their usages" << std::left << std::setw(30) << "help help" << std::endl;
        cout << std::left << std::setw(15) << "ls" << std::left << std::setw(65) << "List all files and directories in the specified directory" << std::left << std::setw(30) << "help ls" << std::endl;
        cout << std::left << std::setw(15) << "cd" << std::left << std::setw(65) << "Change the current working directory" << std::left << std::setw(30) << "help cd" << std::endl;
        cout << std::left << std::setw(15) << "cp" << std::left << std::setw(65) << "Copy files or directories to a specified file or directory" << std::left << std::setw(30) << "help cp" << std::endl;
        cout << std::left << std::setw(15) << "mv" << std::left << std::setw(65) << "Rename or move files" << std::left << std::setw(30) << "help mv" << std::endl;
        cout << std::left << std::setw(15) << "rm" << std::left << std::setw(65) << "Delete files or directories (recursively if not empty)" << std::left << std::setw(30) << "help rm" << std::endl;
        cout << std::left << std::setw(15) << "pwd" << std::left << std::setw(65) << "Display the absolute path of the current working directory" << std::left << std::setw(30) << "help pwd" << std::endl;
        cout << std::left << std::setw(15) << "history" << std::left << std::setw(65) << "Display the command history" << std::left << std::setw(30) << "help history" << std::endl;
        cout << std::left << std::setw(15) << "ps" << std::left << std::setw(65) << "Get detailed information about running processes in the system" << std::left << std::setw(30) << "help ps" << std::endl;
        cout << std::left << std::setw(15) << "tree" << std::left << std::setw(65) << "Display the directory structure in a tree-like format" << std::left << std::setw(30) << "help tree" << std::endl;
        cout << std::left << std::setw(15) << "I/O Redir." << std::left << std::setw(65) << "Redirect I/O of a command to a file" << std::left << std::setw(30) << "help ior" << std::endl;
        std::cout << std::left << std::setw(15) << "Pipe" << std::left << std::setw(65) << "Use the output of one command as the input of another" << std::left << std::setw(30) << "help pipe" << std::endl;
        std::cout << std::left << std::setw(15) << "Background" << std::left << std::setw(65) << "Run a program in the background" << std::left << std::setw(30) << "help bg" << std::endl;
        return 1;
    }

    // ���ݾ������������ϸ�İ�����Ϣ
    if (com_block->para_a == "help")
    {
        printDivider();
        cout << "\033[1;37mhelp\033[0m command is used to display help information for commands." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(25) << "Usage"
             << "\033[0;32m" << left << setw(50) << "Description"
             << "\033[0;32m" << left << setw(20) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(25) << "help" << left << setw(50) << "Display all supported commands for this shell" << left << setw(20) << "help" << endl;
        cout << left << setw(25) << "help [command]" << left << setw(50) << "Get usage information for a specific command" << left << setw(20) << "help ls" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "ls")
    {
        printDivider();
        cout << "\033[1;37mls\033[0m command is used to list all files and directories in a specified directory." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(25) << "Usage"
             << "\033[0;32m" << left << setw(30) << "Description"
             << "\033[0;32m" << left << setw(30) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(25) << "ls [directory path]" << left << setw(30) << "List files and directories in the specified directory" << left << setw(30) << "ls /home/user" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "cd")
    {
        printDivider();
        cout << "\033[1;37mcd\033[0m command is used to change the current working directory." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(25) << "Usage"
             << "\033[0;32m" << left << setw(30) << "Description"
             << "\033[0;32m" << left << setw(30) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(25) << "cd [directory path]" << left << setw(30) << "Change to the specified directory" << left << setw(30) << "cd /home/user" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "cp")
    {
        printDivider();
        cout << "\033[1;37mcp\033[0m command is used to copy files or directories to a specified location." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(25) << "Usage"
             << "\033[0;32m" << left << setw(30) << "Description"
             << "\033[0;32m" << left << setw(30) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(25) << "cp [source path] [destination path]" << left << setw(30) << "Copy files or directories to the specified destination" << left << setw(30) << "cp file.txt /backup/" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "mv")
    {
        printDivider();
        cout << "\033[1;37mmv\033[0m command is used to rename or move files and directories." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(40) << "Usage"
             << "\033[0;32m" << left << setw(40) << "Description"
             << "\033[0;32m" << left << setw(40) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(40) << "mv [file] [file]" << left << setw(40) << "Rename a file" << left << setw(40) << "mv file1.txt file2.txt" << endl;
        cout << left << setw(40) << "mv [file] [directory]" << left << setw(40) << "Move a file to a directory" << left << setw(40) << "mv file1.txt /home/user" << endl;
        cout << left << setw(40) << "mv [same-level dir] [same-level dir]" << left << setw(40) << "Rename a directory" << left << setw(40) << "mv /home/aa /home/rr" << endl;
        cout << left << setw(40) << "mv [nsame-level dir] [nsame-level dir]" << left << setw(40) << "Move a directory to another location" << left << setw(40) << "mv /home/aa /home/rr/uu" << endl
             << endl;
        cout << "\033[0;32m" << left << setw(40) << "Parameter Explanation:"
             << "\033[0m" << endl;
        cout << left << setw(20) << " -b" << left << setw(40) << "Create a backup (_back) of the destination if it already exists before overwriting" << endl;
        cout << left << setw(20) << " -i" << left << setw(40) << "Prompt before overwriting if the source and destination have the same name (y/n)" << endl;
        cout << left << setw(20) << " -n" << left << setw(40) << "Do not overwrite any existing files or directories" << endl;
        cout << left << setw(20) << " -f/none" << left << setw(40) << "When moving, overwrite the destination directly if the source and destination have the same name" << endl
             << endl;
        cout << endl
             << "\033[0;31m" << left << setw(15) << "Note: If absolute paths are not used, it defaults to the current directory."
             << "\033[0;31m"
             << "\033[0m" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "rm")
    {
        printDivider();
        cout << "\033[1;37mrm\033[0m command is used to delete files or directories (recursively if not empty)." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(25) << "Usage"
             << "\033[0;32m" << left << setw(30) << "Description"
             << "\033[0;32m" << left << setw(30) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(25) << "rm [path]" << left << setw(30) << "rm file.txt" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "pwd")
    {
        printDivider();
        cout << "\033[1;37mpwd\033[0m command is used to display the absolute path of the current working directory." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(25) << "Usage"
             << "\033[0;32m" << left << setw(30) << "Description"
             << "\033[0;32m" << left << setw(30) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(25) << "pwd" << left << setw(30) << "pwd" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "history")
    {
        printDivider();
        cout << "\033[1;37mhistory\033[0m command is used to display the command history." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(25) << "Usage"
             << "\033[0;32m" << left << setw(30) << "Description"
             << "\033[0;32m" << left << setw(30) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(25) << "history" << left << setw(30) << "history" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "ps")
    {
        printDivider();
        cout << "\033[1;37mps\033[0m command is used to get detailed information about running processes in the system." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(25) << "Usage"
             << "\033[0;32m" << left << setw(30) << "Description"
             << "\033[0;32m" << left << setw(30) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(25) << "ps" << left << setw(30) << "ps" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "tree")
    {
        printDivider();
        cout << "\033[1;37mtree\033[0m command is used to display the hierarchical structure of files and directories in a tree-like format." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(40) << "Usage"
             << "\033[0;32m" << left << setw(40) << "Description"
             << "\033[0;32m" << left << setw(40) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(40) << "tree [directory]" << left << setw(40) << "Show directory and subdirectory names" << left << setw(40) << "tree /home/aaa" << endl
             << endl;
        cout << "\033[0;32m" << left << setw(40) << "Parameter Explanation:"
             << "\033[0m" << endl;
        cout << left << setw(20) << " -a" << left << setw(40) << "List all files and directories, including hidden ones." << endl;
        cout << left << setw(20) << " -d" << left << setw(40) << "List only directories." << endl;
        cout << left << setw(20) << " -f" << left << setw(40) << "Print the full path prefix of each file." << endl;
        cout << left << setw(20) << " -i" << left << setw(40) << "Omit indentation lines, useful when used with the -f option." << endl;
        cout << left << setw(20) << " None" << left << setw(40) << "List the current directory." << endl
             << endl;
        cout << endl
             << "\033[0;31m" << left << setw(15) << "Multiple parameters can be used together: [-a -d], [-a -d -f], [-a -d -f -i]"
             << "\033[0m" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "ior")
    {
        printDivider();
        cout << "\033[1;37mior\033[0m command is used to perform input/output redirection operations." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(25) << "Usage"
             << "\033[0;32m" << left << setw(55) << "Description"
             << "\033[0;32m" << left << setw(40) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(25) << "[cmd] > [o_file]" << left << setw(55) << "Redirect file descriptor n to file." << left << setw(40) << " tree [dir1] > record.txt" << endl;
        cout << left << setw(25) << "[cmd] >> [o_file]" << left << setw(55) << "Redirect file descriptor n to file in append mode." << left << setw(40) << " tree [dir2] >> record.txt" << endl;
        cout << left << setw(25) << "[cmd] < [i_file]" << left << setw(55) << "Redirect input to file." << left << setw(40) << " pwd < dir.txt / mv change.txt" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "pipe")
    {
        printDivider();
        cout << "The \033[1;37mpipe\033[0m command is used to redirect the output of one command as the input of another command." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(40) << "Usage"
             << "\033[0;32m" << left << setw(40) << "Description"
             << "\033[0;32m" << left << setw(40) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(40) << "[command1] | [command2]" << left << setw(40) << "ls | grep txt" << endl
             << endl;
        printDivider();
    }
    else if (com_block->para_a == "bg")
    {
        printDivider();
        cout << "The \033[1;37mbg\033[0m command is used to run a program in the background." << endl
             << endl;
        cout << "\033[0;32m" << left << setw(40) << "Usage"
             << "\033[0;32m" << left << setw(40) << "Description"
             << "\033[0;32m" << left << setw(40) << "Example"
             << "\033[0m" << endl;
        cout << left << setw(40) << "[command] &" << left << setw(40) << "ps &" << endl
             << endl;
        printDivider();
    }
    else
    {
        cout << "Unknown command. Unable to provide help information." << endl;
        // ���û�и�����򷵻� -1
        return -1;
    }

    return 1;
}

int pwd(Command *com_block)
{
    char buff[255] = {0};
    char *s = getcwd(buff, sizeof(buff));
    if (NULL == s)
    {
        perror("getcwd");
        // ·����ȡʧ���򷵻� -1
        return -1;
    }
    std::cout << buff << std::endl;
    return 1;
}

int mv(Command *com_block)
{
    // �����������洢�����в���
    vector<const char *> args;
    constructCommandArgs(com_block, args);

    // ִ����������
    execvp(args[0], const_cast<char *const *>(args.data()));

    // �ļ���ʧ�ܴ���
    perror("execvp");
    // �����ϵͳ����ý������쳣����ֹ
    exit(1);
}




    
int ls(Command *com_block) {
    std::string option = com_block->option; // 获取选项
    std::string path = com_block->para_a; // 获取路径

    // 若路径为空，则默认路径为本地路径
    if (path.empty()) {
        path = ".";
    }

    // 打开指定路径
    DIR *dir = opendir(path.c_str());
    if (dir == NULL) {
        perror("opendir");
        return -1;
    }

    // 读取目录中的文件和子目录
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        // 根据选项进行过滤
        if (option.find("a") == std::string::npos && name[0] == '.') {
            continue; // 不显示隐藏文件
        }
        if (option.find("d") == std::string::npos && entry->d_type == DT_DIR) {
            continue; // 不显示目录
        }

        // 输出文件或目录名
        std::cout << name << std::endl;
    }

    // 关闭目录
    closedir(dir);

    return 0;
}



int history(Command *com_block) {
    std::string option = com_block->option; // 获取选项
    if (option == "-r") {
        // 从文件中读取历史记录
        std::ifstream file("history.txt");
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
            file.close();
        } else {
            std::cerr << "Failed to open history file" << std::endl;
        }
    } else if (option == "-w") {
        // 将历史记录写入文件
        std::ofstream file("history.txt", std::ios::app);
        if (file.is_open()) {
            while (!com_block->commandStack.empty()) {
                file << com_block->commandStack.top() << std::endl;
                com_block->commandStack.pop();
            }
            file.close();
        } else {
            std::cerr << "Failed to open history file" << std::endl;
        }
    } else {
        // 打印命令历史记录
        std::stack<std::string> tempStack;
        if (!com_block->commandStack.empty()) {
             tempStack = com_block->commandStack;
            while (!tempStack.empty()) {
        std::cout << tempStack.top() << std::endl;
        tempStack.pop();
            }
        }   
    }
    return 0;
}

int rm(Command* com_block) {
    std::string option = com_block->option; // 获取选项
    bool interactive = false;
    bool force = false;
    bool recursive = false;
    
    // 解析选项
    for (char c : option) {
        switch (c) {
            case 'i':
                interactive = true; // 交互式删除
                break;
            case 'f':
                force = true; // 强制删除
                break;
            case 'r':
                recursive = true; // 递归删除
                break;
            default:
                break;
        }
    }
    
    // 获取要删除的文件名列表
    std::vector<std::string> filenames;
    filenames.push_back(com_block->para_a);
    if (!com_block->para_b.empty()) {
        filenames.push_back(com_block->para_b);
    }
    
    // 逐一删除文件
    for (const std::string& filename : filenames) {
        if (interactive) {
            // 逐一询问确认
            std::cout << "Do you want to delete " << filename << "? (y/n): ";
            std::string answer;
            std::cin >> answer;
            if (answer != "y" && answer != "Y") {
                continue;
            }
        }
        
        if (force) {
            // 强制删除，无需确认
            std::remove(filename.c_str());
        } else {
            // 检查文件是否存在
            if (std::ifstream(filename)) {
                // 文件存在，进行删除确认
                std::cout << "Are you sure you want to delete " << filename << "? (y/n): ";
                std::string answer;
                std::cin >> answer;
                if (answer == "y" || answer == "Y") {
                    std::remove(filename.c_str());
                }
            } else {
                std::cerr << "File " << filename << " does not exist." << std::endl;
            }
        }
        
        if (recursive) {
            // 递归删除目录及以下文件
            std::string command = "rm -r " + filename;
            std::system(command.c_str());
        }
    }
    com_block->commandStack = std::stack<std::string>(); // 清空命令栈
    return 0;
}
