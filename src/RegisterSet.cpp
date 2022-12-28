#include "RegisterSet.h"
#include "UcodeCollection.h"

RegisterSet::RegisterSet() {}
RegisterSet::~RegisterSet() {}

void RegisterSet::increaseProgramCounter() {
    this->program_counter++;
}

int RegisterSet::getProgramCounter() {
    return this->program_counter;
}

stack<int> RegisterSet::getGpRegister() {
    return this->gp_register;
}

void RegisterSet::setProgramCounter(int addr) {
    this->program_counter = addr;
}

void RegisterSet::pushGpRegister(int data) {
    gp_register.push(data);
}

int RegisterSet::popGpRegister() {
    if (gp_register.empty()) {
        throw UcodeRuntimeException("���� �������Ϳ��� ������ �߻��߽��ϴ�", -1);
    }
    int data = gp_register.top();
    gp_register.pop();
    return data;
}
