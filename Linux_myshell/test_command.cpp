#include <iostream>
#include <cassert>
#include "command.cpp"

void test_tune()
{
    // Test case 1: Valid command
    Command com_block;
    com_block.para_a = "ls";
    com_block.para_b = "wc";
    com_block.in = nullptr;
    com_block.out = nullptr;
    int result = tune(&com_block);
    assert(result == 0);
    std::cout << "Test case 1 passed." << std::endl;

    // Test case 2: Invalid command
    Command com_block2;
    com_block2.para_a = "invalid_command";
    com_block2.para_b = "wc";
    com_block2.in = nullptr;
    com_block2.out = nullptr;
    result = tune(&com_block2);
    assert(result == -1);
    std::cout << "Test case 2 passed." << std::endl;

    // Test case 3: Invalid pipe creation
    Command com_block3;
    com_block3.para_a = "ls";
    com_block3.para_b = "wc";
    com_block3.in = nullptr;
    com_block3.out = nullptr;
    // Modify pipe creation to fail
    // BEGIN: Modify pipe creation
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return -1;
    }
    // END: Modify pipe creation
    result = tune(&com_block3);
    assert(result == -1);
    std::cout << "Test case 3 passed." << std::endl;
}

int main()
{
    test_tune();
    return 0;
}