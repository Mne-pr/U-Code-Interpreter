#define _CRT_SECURE_NO_WARNINGS
#include "Logger.h"
#include <ctime>
#include <fstream>
#include <string>
#include <Windows.h>

using namespace std;

Logger::Logger(string path, int *breakpoints, int breakpoint_num) : breakpoints_(breakpoints), breakpoint_num_(breakpoint_num), name(path), start_addr_(0), data_area_size_(0), cycle_count(0) {
    this->opcode_count = new int[37]();
    time_t ttime = time(0);
    struct tm local_time;
    localtime_s(&local_time, &ttime);
    this->execution_date = to_string(local_time.tm_year + 1900) + "-" + to_string(local_time.tm_mon + 1)
        + "-" + to_string(local_time.tm_mday) + " " + to_string(local_time.tm_hour)
        + ":" + to_string(local_time.tm_min) + ":" + to_string(local_time.tm_sec);
    //
    Printer.ScreenInit();
}

void Logger::setLength(int length) { this->length_ = length; }
void Logger::setStartAddr(int start_addr) { this->start_addr_ = start_addr; }
void Logger::setDataAreaSize(int data_area_size) { this->data_area_size_ = data_area_size; }
void Logger::setLabels(vector<Label> labels) { this->labels_ = labels; }
void Logger::setInstructions(vector<Instruction> instrcutions) { this->instructions_ = instrcutions; }
void Logger::setUCodes(vector<UCode> u_codes) { this->u_codes_ = u_codes; }

void Logger::increaseOpcodeCount(Opcode opcode) {
    cycle_count++;
    opcode_count[(int)opcode]++;
    if (opcode == UJP || opcode == TJP || opcode == FJP || opcode == CALL) { branch_count++; }
}

void Logger::initWindowBuffer() {
    char breakpoint_buffer[10];
    char object_buffer[1024];
    char line_buffer[10000];
    window_buffer.push_back("┌───────┬────────────────┬───────────┬────────┬──────────┬──────────┬─────┐");
    window_buffer.push_back("│ line  │     object     │   Lable   │ OPCode │ Operand1 │ Operand2 │ now │");
    
    for (int line = 0; line < instructions_.size(); line++) {
        strcpy(breakpoint_buffer, "  ");
        for (int i = 0; i < breakpoint_num_; i++) {
            if (breakpoints_[i] == line + 1) {
                strcpy(breakpoint_buffer, "●");
                break;
            }
        }
        window_buffer.push_back("├───────┼────────────────┼───────────┼────────┼──────────┼──────────┼─────┤");
        if (instructions_[line].operand1 == -1) {
            sprintf(object_buffer, "%d", instructions_[line].opcode);
        }
        else if (instructions_[line].operand2 == -1) {
            sprintf(object_buffer, "%d, %d", instructions_[line].opcode, instructions_[line].operand1);
        }
        else {
            sprintf(object_buffer, "%d, %d, %d", instructions_[line].opcode, instructions_[line].operand1, instructions_[line].operand2);
        }
        sprintf(line_buffer, "│%s%4d │ (%-13s)│ %-10s│ %-7s│ %-9s│ %-9s│", breakpoint_buffer, line + 1, object_buffer, u_codes_[line].label.c_str(), u_codes_[line].opcode.c_str(), u_codes_[line].operand1.c_str(), u_codes_[line].operand2.c_str());
        window_buffer.push_back(line_buffer);
    }
    window_buffer.push_back("└───────┴────────────────┴───────────┴────────┴──────────┴──────────┴─────┘");
}

void Logger::updateWindow(int idx, int program_counter, stack<int> stack_area, stack<int> gp_register) {
    //system("cls");
    Printer.ScreenClear();
    Printer.SetPrintClear();

    Printer.ScreenRender("─ console ────────────────────────────────────────────────────────────────");
    Printer.ScreenRender(out_buffer.c_str());
    Printer.ScreenRender("──────────────────────────────────────────────────────────────────────────");

    if (idx != -1) {

        char pc_buffer[1024];
        char sa_buffer[1024];
        char gp_buffer[1024];
        strcpy(sa_buffer, " ");
        strcpy(gp_buffer, " ");
        sprintf(pc_buffer, "%5d ", program_counter);
        for (stack<int> dump = stack_area; !dump.empty(); dump.pop()) {
            strcat(sa_buffer, to_string(dump.top()).c_str());
            strcat(sa_buffer, " ");
        }
        for (stack<int> dump = gp_register; !dump.empty(); dump.pop()) {
            strcat(gp_buffer, to_string(dump.top()).c_str());
            strcat(gp_buffer, " ");
        }

        Printer.AddTemp("┌──────┬──────┐┌────────────┬"); for (int i = 0; i < strlen(sa_buffer); i++) { Printer.AddTemp("─"); }
        Printer.AddTemp("┐┌─────────────┬"); for (int i = 0; i < strlen(gp_buffer); i++) { Printer.AddTemp("─"); } Printer.AddTemp("┐");
        Printer.ScreenRender();

        Printer.AddTemp("│  PC  │"); Printer.AddTemp(pc_buffer);
        Printer.AddTemp("││ Stack Area │"); Printer.AddTemp(sa_buffer);
        Printer.AddTemp("││ GP Register │"); Printer.AddTemp(gp_buffer); Printer.AddTemp("│");
        Printer.ScreenRender();

        Printer.AddTemp("└──────┴──────┘└────────────┴"); for (int i = 0; i < strlen(sa_buffer); i++) { Printer.AddTemp("─"); }
        Printer.AddTemp("┘└─────────────┴"); for (int i = 0; i < strlen(gp_buffer); i++) { Printer.AddTemp("─"); } Printer.AddTemp("┘");
        Printer.ScreenRender();

    }

    for (int line = 0; line < window_buffer.size(); line++) {
        if(line <= 2) Printer.ScreenRender(window_buffer[line].c_str());
        if (line > 2 && line % 2 == 1) {
            Printer.AddTemp(window_buffer[line].c_str());
            if (line == 3+idx*2) {
                Printer.AddTemp("  <  │");
            } else {
                Printer.AddTemp("     │");
            }
            Printer.ScreenRender();
        }
        if (line == window_buffer.size() - 1) Printer.ScreenRender(window_buffer[line].c_str());
    }

    if (idx == -1) {
        Sleep(300);
        Printer.ScreenRender("시뮬레이션 종료. 엔터키로 다음단계 진행");
        Printer.ScreenSwitch();
        while (1) {
            int key = _getch();
            if (key == 13) { Printer.ScreenRelease(); return; }
        }
    }

    if (idx != -1) {
        for (int i = 0; i < breakpoint_num_; i++) {
            if (breakpoints_[i] == program_counter-1) {
                //system("pause"); 왜그런지 모르겠지만 이거때문에 진행불가능
                Sleep(300);
                Printer.ScreenRender("브레이크포인트. 엔터키로 진행");
                Printer.ScreenSwitch();
                while (1) {
                    int key = _getch();
                    if (key == 13) break;
                }
                break;
            }
        }
        Sleep(300);
    }

    Printer.ScreenSwitch();
}

void Logger::updateOutBuffer(string out) {
    out_buffer += out;
}

void Logger::storeLog(ofstream& log_file) {
    vector<string> buffer;
    char object_buffer[1024];
    char line_buffer[10000];

    if (log_file.fail()) {
        cerr << "로그파일 저장 불가" << endl;
    }

    buffer.push_back("U-Code Interpreter: successfully executed.");
    buffer.push_back("");
    buffer.push_back("U-Code Name: "); buffer.back() += this->name;
    buffer.push_back("Date of execution: "); buffer.back() += this->execution_date;
    buffer.push_back("");
    buffer.push_back("====================  U - Code Infomation  ====================");
    buffer.push_back("line  object          Lable       OPCode  Operand1  Operand2");
    for (int line = 0; line < instructions_.size(); line++) {
        if (instructions_[line].operand1 == -1) {
            sprintf(object_buffer, "%d", instructions_[line].opcode);
        }
        else if (instructions_[line].operand2 == -1) {
            sprintf(object_buffer, "%d, %d", instructions_[line].opcode, instructions_[line].operand1);
        }
        else {
            sprintf(object_buffer, "%d, %d, %d", instructions_[line].opcode, instructions_[line].operand1, instructions_[line].operand2);
        }
        sprintf(line_buffer, "%4d  (%-12s)  %-10s  %-6s  %-8s  %-8s", line+1, object_buffer, u_codes_[line].label.c_str(), u_codes_[line].opcode.c_str(), u_codes_[line].operand1.c_str(), u_codes_[line].operand2.c_str());
        buffer.push_back(line_buffer);
    }
    buffer.push_back("");
    buffer.push_back("======================  Output Result  ======================");
    buffer.push_back(out_buffer);
    buffer.push_back("");
    buffer.push_back("========================  Statistics  ========================");
    buffer.push_back("U-Code Length: "); buffer.back() += to_string(this->length_);
    buffer.push_back("Program Begin Line: "); buffer.back() += to_string(this->start_addr_);
    buffer.push_back("Static Data Area Size: "); buffer.back() += to_string(this->data_area_size_);
    buffer.push_back("Total Execution Cycle: "); buffer.back() += to_string(this->cycle_count);
    buffer.push_back("Total number of branches: "); buffer.back() += to_string(this->branch_count);
    buffer.push_back("");
    buffer.push_back("Label List:");
    for (int idx = 0; idx < this->labels_.size(); idx++) {
        buffer.push_back(labels_[idx].name); buffer.back() += ", "; buffer.back() += to_string(labels_[idx].index);
    }
    buffer.push_back("");
    buffer.push_back("Executed OPcode Count: ");
    for (int i = 0; i < 37; i++) {
        if (i % 8 == 0) { buffer.push_back(""); }
        buffer.back() = buffer.back() + OPCODE[i] + " = " + to_string(this->opcode_count[i]) + ", ";
    }

    for (int i = 0; i < buffer.size(); i++) {
        log_file << buffer[i] << endl;
    }
    log_file.close();
    cout << "로그파일(*.lst) 이(가) 성공적으로 저장되었음" << endl;
    system("pause");
}