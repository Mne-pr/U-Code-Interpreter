#pragma once
#include <stack>

using namespace std;

class RegisterSet {
public:
    RegisterSet();
    ~RegisterSet();

    void increaseProgramCounter();
    int getProgramCounter();
    stack<int> getGpRegister();
    void setProgramCounter(int addr);

    void pushGpRegister(int data);
    int popGpRegister();

private:
    stack<int> gp_register; // ���� ��������, ����
    int program_counter; // ���α׷� ī����
};