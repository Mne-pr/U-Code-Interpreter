#include <iostream>
#include "DoubleBufferPrinter.h"
using namespace std;

DoubleBufferPrinter::DoubleBufferPrinter() {}
DoubleBufferPrinter::~DoubleBufferPrinter() {}

void DoubleBufferPrinter::ScreenInit() {
	screen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	screen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	//CURSOR.dwSize = 1;
	//CURSOR.bVisible = false;
	//SetConsoleCursorInfo(screen[0], &CURSOR);
	//SetConsoleCursorInfo(screen[1], &CURSOR);
}

//void DoubleBufferPrinter::ScreenRender(char* text, int nextline) {
//	if (nextline == 1) { this->printX = 0; this->printY++; }
//	DWORD dw;
//	COORD backBufferCursor = { printX,printY };
//	SetConsoleCursorPosition(screen[whatScreen], backBufferCursor);
//	WriteFile(screen[whatScreen], text, strlen(text), &dw, NULL);
//	printX += strlen(text);
//}

void DoubleBufferPrinter::ScreenRender(const char* text) {
	DWORD dw;
	COORD backBufferCursor = { 0,printY };
	SetConsoleCursorPosition(screen[whatScreen], backBufferCursor);
	WriteFile(screen[whatScreen], (char*)text, strlen(text), &dw, NULL);
	printY++;
}

void DoubleBufferPrinter::ScreenRender() {
	DWORD dw;
	COORD backBufferCursor = { 0,printY };
	SetConsoleCursorPosition(screen[whatScreen], backBufferCursor);
	WriteFile(screen[whatScreen], this->temp, strlen(this->temp), &dw, NULL);
	printY++;
	temp[0] = '\0';
}

// printX 제거할지 생각해볼 필요 있음


void DoubleBufferPrinter::ScreenSwitch() {
	SetConsoleActiveScreenBuffer(screen[whatScreen]);
	if (whatScreen == 0) whatScreen = 1;
	else whatScreen = 0;
}

void DoubleBufferPrinter::ScreenClear() {
	COORD coor = { 0,0 };
	DWORD dw;
	FillConsoleOutputCharacter(screen[whatScreen], ' ', 100*100, coor, &dw);
}

void DoubleBufferPrinter::ScreenRelease() {
	try {
		CloseHandle(screen[0]);
		CloseHandle(screen[1]);
	}
	catch (exception) { cout << "어쨋거나 프로그램 종료"; return; }
}

void DoubleBufferPrinter::SetPrintClear() {
	printX = 0;
	printY = 0;
	temp[0] = '\0';
}

void DoubleBufferPrinter::AddTemp(const char* text) {
	strcat(temp,text);
}

void DoubleBufferPrinter::SetScreenSize() {
	//char command[50];
	//sprintf(command, "mode con:cols=100 lines=%d", printY + 2);
	//system("mode con:cols=100 lines=50");
}