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
/// 노트 카운트랑 노트 초기위치 SetNotePosition 함수에서 초기화 함
/// </summary>
int noteCount;
int missCount = 0;
int combo = 0;


/// <summary>
/// 파일 경로는 프로젝트 폴더 안에 넣어두기만 해도 자동으로 읽어오더라
/// 프로젝트 폴더 안에 두고 파일명을 지정해주면 됨
/// exe 로 실행할때엔 디버그 폴더안에도 둬야함
/// </summary>
const char* asciiArtFilePath1 = "ascii_art_";
const char* asciiArtFilePath2 = "ascii_art_ ";




/// <summary>
/// 색깔 팔레트
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
