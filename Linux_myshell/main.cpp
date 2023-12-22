
#include "shell.h"
#include "command.h"
#include <vector>
#include <iostream>
#include <stack>

using namespace std;

int main(int argc, char* argv[]) {

    string present_dir = argv[0];  // 当前路径
    stack<string> commandStack;
    while (true) {
        char currentPath[100];// 存储当前路径的缓冲区
        char command[256];       
        // 获取当前路径
        if (getcwd(currentPath, sizeof(currentPath)) == NULL){
            perror("Error getting current path");
            return 1;
        }
        // 显示用户输入提示
        printf("%s $ ",currentPath);

        // 从标准输入读取用户输入的命令
        if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("Error reading command");
            return 1;
        }

        // 将命令压入栈中
        commandStack.push(command);

        int com_counts = 1, i = 0;
        while (command[i] != '\0') {
            if (command[i] == '|') {
                com_counts++;
            }
            i++;
        }

        vector<Command> com_list(com_counts);  

        if (commmadSplit(command, com_list, com_counts) == -1) {
            continue;
        }

        i = 0;
        const char* last = NULL;
        int result = 0;

        while (i < com_counts) {
            com_list[i].in = last;
            // 输入重定向
            if (com_list[i].is_in_relocate == true || com_list[i].is_out_relocate == true) { 
                result = fv[0](&com_list[i]); // 0->Redirection
                if(i >= 0 && i != com_counts -1){
                   last = com_list[i].out;
                }
            }
            // 执行命令          
            else {
                last = run(&com_list[i], i, com_counts, result);  // Pass a pointer to the current Command
            }
            if (result < 0) {
                break;
            }
            i++;
        }
        if (strcmp(command, "exit\n") == 0) {
        break;  // 退出循环
    }
    }

    return 0;
}

//char command[] = "list -l-a>> hell.txt &  | move -l-h wtf.txt << llol.txt & ";

//commmadSplit(command, com_list,com_counts);
//Command* com_list = new Command[com_counts];
//cout<<"a="<<com_list[0].para_a<<" b="<<com_list[0].para_b<<" r="<<com_list[0].relocated_file<<'\n';
//cout<<"a="<<com_list[1].para_a<<" b="<<com_list[1].para_b<<" r="<<com_list[1].relocated_file<<'\n';
