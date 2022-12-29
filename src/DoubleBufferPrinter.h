#pragma once
#include <Windows.h>
#include <cstring>
#include <conio.h>

class DoubleBufferPrinter {
public:
	DoubleBufferPrinter();
	~DoubleBufferPrinter();
	public:
	void ScreenInit();
	//void ScreenRender(char* text, int nextline);
	void ScreenRender(const char* text);
	void ScreenRender();
	void ScreenSwitch();
	void ScreenClear();
	void ScreenRelease();
	void SetPrintClear();
	void SetScreenSize();
	void AddTemp(const char* text);
private:
	HANDLE screen[2];
	CONSOLE_CURSOR_INFO CURSOR;
	int whatScreen = 0;
	int printX, printY;
	char temp[200];

};