#pragma once
#include "input.h"
#include <string.h>

void ScreenDrawUpArrow(COORD pos, int color);
void setColor(int, int);
void ScreenDrawKeyInterface();
void UpdateNote();
void GenerateNote();
void UpdateNotePosition(int);
void ScreenPrint(int x, int y, const char* str, int length);

int l_note[] = { 1,0,1,0,1,1 };
COORD NotecurPos_l[] = { {8,50},{8,50},{8,50},{8,50},{8,50},{8,50} };
COORD NoteprePos_l[] = { {8,50},{8,50},{8,50},{8,50},{8,50},{8,50} };

/// <summary>
/// »ö±ò ÆÈ·¹Æ®
/// </summary>
const int color_black = 0;
const int color_white = 15;
const int color_green = 10;
const int color_red = 12;
const int color_yellow = 6;
const int color_dark_white = 7;
const int color_blue = 9;
const int color_gray = 8;



//#include <stdio.h>
//#include <windows.h>
//#include <stdbool.h>