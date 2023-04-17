#include "input.h"

const int ESCAPE = 0;
const int LEFT = 1;
const int RIGHT = 2;
const int UP = 3;
const int DOWN = 4;
const int SPACE = 5;



#define MAX_KEY = 6;
// Ű�� �ԷµǾ�����! �ԷµǾ��ٸ� true
bool keyTable[6];


/// ��, ��, ��, �� �� �ش� keyTable ���� �޾ƿ´�
bool GetKeyTable(const int keyIndex)
{
	return keyTable[keyIndex];
}

/// Ű ���� �� ������ �ٲ��ִ� �Լ�
void SetKeyTable(const int keyIndex, bool isOn)
{
	keyTable[keyIndex] = isOn;
}


/// GetAsyncKeyState ���
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