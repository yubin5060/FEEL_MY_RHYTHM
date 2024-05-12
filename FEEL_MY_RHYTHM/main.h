#pragma once
//#include "input.h"
//#include "audio.h"
#include <string.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") 


void GameLogic(int num, unsigned run_time, int number_hitobjects);
void ScreenDrawUpArrow(COORD pos, int color);
void setColor(int, int);
void ScreenDrawKeyInterface();
HANDLE GetScreenHandle();
void ScreenPrint(int x, int y, const char* str, int length);
void PrintAsciiArt(char**, int , int , int , int, int);
void PlayAnim1(int, int, int);
int GetSelectedMenu();
void DrawMenuList(int);
void PlayAnim2(int, int, int);
void PlayAnim3(int, int, int);
void FindAsciiArt(const char* asciiArtFilePath, int fileNum, int asciiNum);
void PrintCombo(int, int, int);

/// <summary>
/// ��Ʈ ī��Ʈ�� ��Ʈ �ʱ���ġ SetNotePosition �Լ����� �ʱ�ȭ ��
/// </summary>
int noteCount;
int missCount = 0;
int combo = 0;


/// <summary>
/// ���� ��δ� ������Ʈ ���� �ȿ� �־�α⸸ �ص� �ڵ����� �о������
/// ������Ʈ ���� �ȿ� �ΰ� ���ϸ��� �������ָ� ��
/// exe �� �����Ҷ��� ����� �����ȿ��� �־���
/// </summary>
const char* asciiArtFilePath1 = "ascii_art_";
const char* asciiArtFilePath2 = "ascii_art_ ";




/// <summary>
/// ���� �ȷ�Ʈ
/// </summary>
/// 
enum Color
{
	color_black,
	color_dark_blue,
	color_dark_green,
	color_dark_aqua,
	color_dark_red,
	color_dark_purple,
	color_dark_yellow,
	color_dark_white,
	color_gray,
	color_blue,
	color_green,
	color_aqua,
	color_red,
	color_purple,
	color_yellow,
	color_white
};
