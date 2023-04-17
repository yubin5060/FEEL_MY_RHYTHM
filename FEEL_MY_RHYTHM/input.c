#include "input.h"

const int ESCAPE = 0;
const int LEFT = 1;
const int RIGHT = 2;
const int UP = 3;
const int DOWN = 4;
const int SPACE = 5;



#define MAX_KEY = 6;
// 키가 입력되었는지! 입력되었다면 true
bool keyTable[6];


/// 상, 하, 좌, 우 중 해당 keyTable 값을 받아온다
bool GetKeyTable(const int keyIndex)
{
	return keyTable[keyIndex];
}

/// 키 값의 참 거짓을 바꿔주는 함수
void SetKeyTable(const int keyIndex, bool isOn)
{
	keyTable[keyIndex] = isOn;
}


/// GetAsyncKeyState 사용
void UpdateInput()
{
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		SetKeyTable(ESCAPE, true);
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		SetKeyTable(SPACE, true);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		SetKeyTable(LEFT, true);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		SetKeyTable(RIGHT, true);
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		SetKeyTable(UP, true);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		SetKeyTable(DOWN, true);
	}

}