#define _CRT_SECURE_NO_WARNINGS

//#define MAX_NOTE 6
#define ROW_SIZE 160
#define COLUMN_SIZE 50
#define MAX_LINES 5000


#include "input.h"
#include "main.h"
#include "audio.h"




///������� �ε��ְ� ���� ����ü�� �Լ�
typedef struct
{
	int x;
	double y;
	int time;
	int type;
	int hit_sound;
} HitObject;

// ���
int scrollSpeed = 1500;
int judgeLineY = 2;
int yHeight = 50;


// ����
HitObject hitobjects[3][MAX_LINES];
int num_hitobjects = 0;
int num_hitobjects2 = 0;
int num_hitobjects3 = 0;

int startNote = 0;


char* filename_fanclub = "fanclub.osu";
char* filename_hypeboy = "Hype Boy.osu";
char* filename_kamui = "Kamui.osu";
char* filename_delarue = "DeLaRue.osu";

FILE* fp1;
FILE* fp2;
FILE* fp3;


char buffer[1024];
char buffer2[1024];
char buffer3[1024];

char* token;



// ����
// 
// Ÿ��Ʋ ��� ���� ���� ����
bool bTitle = true;
bool bMenu = true;
bool bPlaySound = true;
int selectedNum = 0;






///
/// 04. 10
/// Ŀ�� �ޱ� gotoxy, initConsole ��
/// â ������ ����
/// ***���� ���۸� ã�ƺ���***
/// 






// ȭ�� ���۰� �޾ƿ� ����ü (�簢������ ���� �����Ѵٰ� ��)
SMALL_RECT consoleScreenSize;
// UI �ִ� ������ ������
SMALL_RECT _UImaxSize;
// ����(ȭ��) 2�� ����
HANDLE hScreen[2];
bool bScreenIndex;

// ����׿� ��ġ ��� ����
int g_ms_ForDebugA[255] = { 0, };
int g_ms_ForDebugAd[255] = { 0, };

int g_ms_ForDebugB[255] = { 0, };
int g_ms_ForDebugBd[255] = { 0, };

int g_ms_ForDebug2[255] = { 0, };
int g_ms_ForDebug2d[255] = { 0, };

ULONGLONG g_ms_ForDebug_OriginalElaspedTime[10000] = { 0, };



// song1 �� elapsed Time
double g_elapsedTimeA = 0.f;
double g_elapsedTimeB = 0.f;

double g_elapsedTime_Note = 0.f;

double g_deltaTime = 0.f;


/// <summary>
/// �ʱ�ȭ
/// ���� �ޱ� -> ���߿� �ܼ�â ũ�� ���� �����ϱ�
/// </summary>
void initConsole()
{
	system("mode con cols=160 lines=50 | title FEEL_THE_RHYTHM");
	// ���� ���۸�.. ���� �ΰ� �����
	hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	// Ŀ�� ���ܹ����� (�ȱ�����! �Ⱥ���!)
	HANDLE consonleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO consoleCursor = { 0, };
	consoleCursor.bVisible = 0;											// 0 �̸� Ŀ���� ����, 0 �� �ƴ� ��쿡 ���δ�
	consoleCursor.dwSize = 1;											// ���� ���� Ŀ���� ũ�⸦ �����Ѵ�

	// Ŀ�������� �������� �� ȭ�鿡 �Է�
	SetConsoleCursorInfo(consonleHandle, &consoleCursor);
	SetConsoleCursorInfo(hScreen[0], &consoleCursor);
	SetConsoleCursorInfo(hScreen[1], &consoleCursor);


	// â ũ�� ����
	HWND hWnd = GetConsoleWindow();
	LONG style = GetWindowLong(hWnd, GWL_STYLE);
	style &= ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX);
	SetWindowLong(hWnd, GWL_STYLE, style);

	// �ܼ� â ũ�� ����
	SMALL_RECT screenRect = { 0,0,ROW_SIZE - 1,COLUMN_SIZE - 1};
	SetConsoleWindowInfo(consonleHandle, TRUE, &screenRect);
	SetConsoleWindowInfo(hScreen[0], TRUE, &screenRect);
	SetConsoleWindowInfo(hScreen[1], TRUE, &screenRect);

	// ȭ�� ���� ũ�� ����
	COORD bufferSize = { ROW_SIZE,COLUMN_SIZE };
	SetConsoleScreenBufferSize(consonleHandle, bufferSize);
	SetConsoleScreenBufferSize(hScreen[0], bufferSize);
	SetConsoleScreenBufferSize(hScreen[1], bufferSize);


	// ȭ�� ������ �޾ƿ��� (���۰� �޾Ƽ� ScreenSize �� ����)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	GetConsoleScreenBufferInfo(hScreen[0], &csbi);
	GetConsoleScreenBufferInfo(hScreen[1], &csbi);
	consoleScreenSize.Left = csbi.srWindow.Left;
	consoleScreenSize.Right = csbi.srWindow.Right;
	consoleScreenSize.Bottom = csbi.srWindow.Bottom;
	consoleScreenSize.Top = csbi.srWindow.Top;

	_UImaxSize.Left = consoleScreenSize.Left + 2;
	_UImaxSize.Right = consoleScreenSize.Right - 2;
	_UImaxSize.Bottom = consoleScreenSize.Bottom - 2;
	_UImaxSize.Top = consoleScreenSize.Top + 2;


	// ������ �ʱ�ȭ
	g_elapsedTimeA = 0.f;
	g_elapsedTimeB = 0.f;

	g_elapsedTime_Note = 0.f;
}

HANDLE GetScreenHandle()
{
	int index = (bScreenIndex ? 1 : 0);

	return hScreen[bScreenIndex];
}


void SetStdScreen()
{
	SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
}

void SetBackScreen()
{
	SetConsoleActiveScreenBuffer(GetScreenHandle());
}


/// ���� �ΰ��� ��ȯ���ִ� �Լ�
void ScreenFlipping()
{
	SetConsoleActiveScreenBuffer(GetScreenHandle());
	bScreenIndex = !bScreenIndex;
}



/// ȭ���� ���� �Լ�. ���� ���۷� ��ȯ�� �� ���� ��³����� �������Ѵ�
void ScreenClear()
{
	COORD coordScreen = { 0, 0 };
	DWORD dwConSize = 8000;
	DWORD dwWrittenCount = 0;

	// �ܼ� â�� �������� ä���
	FillConsoleOutputCharacterW(GetScreenHandle(), L' ', dwConSize, coordScreen, &dwWrittenCount);

	// ���� �Ӽ��� �����Ͽ� �ܼ� â�� �����
	WORD wColors = ((WORD)0 << 4) | (WORD)15; // ��� ���ڻ�, ���� ����
	FillConsoleOutputAttribute(GetScreenHandle(), wColors, dwConSize, coordScreen, &dwWrittenCount);
}


/// ���ۿ� �׸��׸��� (���� �� ĭ ���) ��Ʈ(ȭ��ǥ) ��¿� ���簢��
void ScreenPrint(int x, int y, const char* str, int length)
{
	DWORD dw;	// unsigned long ����ü
	COORD CursorPosition = { x, y };

	SetConsoleCursorPosition(GetScreenHandle(), CursorPosition);
	WriteFile(GetScreenHandle(), &str, length, &dw, NULL);
}


///
/// �ٽ� ����
/// �� ���ڿ� ��¿�..gg
void ScreenDraw(int x, int y, const char* str)
{
	DWORD dw;
	COORD Cur = { x, y };

	SetConsoleCursorPosition(GetScreenHandle(), Cur);

	// ���ڿ��� ���̸� for ������ ����Ͽ� ���
	int strLen = 0;
	while (str[strLen] != '\0')
	{
		strLen++;
	}

	WriteFile(GetScreenHandle(), str, strLen, &dw, NULL);
}

/// ���� �ݱ�
void ScreenRelease()
{
	CloseHandle(hScreen[0]);
	CloseHandle(hScreen[1]);
}

/// <summary>
/// Ŀ�� ��ġ �̵��ϴ� �Լ�
/// y ��ǥ�� �Ʒ��� ������ ����, ������ ���� ���
/// </summary>
/// <param name="x">�̵��� x��ǥ</param>
/// <param name="y">�̵��� y��ǥ</param>
void gotoXY(int x, int y)
{
	COORD cursor = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
}


///
/// 04. 10
/// ��ǲ ���� ������
/// ��ĥ�� �Ǵ� UI
/// ���� ��üȭ��
/// 

// ��� ��, �۲� �� ����
void setColor(int backColor, int textColor)
{
	//textColor = color_white;
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (backColor << 4) + textColor);
	SetConsoleTextAttribute(GetScreenHandle(), (backColor << 4) + textColor);
}


/// <summary>
/// ȭ��ǥ ��� ���� (UP KEY)
/// </summary>
const int arr[5][5] =
{
	{0,0,1,0,0},
	{0,1,1,1,0},
	{1,0,1,0,1},
	{0,0,1,0,0},
	{0,0,1,0,0}
};


/// <summary>
/// �ʹ� �������ϰ� �پ��� ���(���߿� ���� �ö�� ��Ʈ ��¿��� ����ϴ�)
/// ���� ���ຼ��
/// �����¿� ȭ��ǥ ��� �Լ��̴�
/// </summary>
/// <param name="pos">��ǥ ������ ����ִ� ����ü</param>
/// <param name="color">���� ������ ����ұ�?</param>
void DrawUpArrow(COORD pos, int color)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			// j �� ���� ��� (x), i �� ���� ����� ��� (y)
			// ���δ� ���κ��� �ι� ����ؾ���.. -> j*2
			gotoXY(pos.X + 2 * j, pos.Y + i);
			if (arr[i][j] == 0)
			{
				setColor(color_black, 15);
			}
			if (arr[i][j] == 1)
			{
				setColor(color, 15);
				printf("  ");
			}
		}
	}
}



void ScreenDrawUpArrow(COORD pos, int color)
{
	//const char* c = '  ';
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			//ScreenPrint(pos.X + 2 * j, pos.Y + i, NULL);
			////gotoXY(pos.X + 2 * j, pos.Y + i);
			//if (arr[i][j] == 0)
			//{
			//	setColor(color_black, 15);
			//}
			if (arr[i][j] == 1)
			{
				setColor(color, 15);
				ScreenPrint(pos.X + 2 * j, pos.Y + i, '  ', 2);
				//printf("  ");
			}
		}
	}
}

void DrawLeftArrow(COORD pos, int color)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			gotoXY(pos.X + 2 * i, pos.Y + j);
			if (arr[i][j] == 0)
			{
				setColor(color_black, 15);
			}
			if (arr[i][j] == 1)
			{
				setColor(color, 15);
				printf("  ");
			}
		}
	}
}

void ScreenDrawLeftArrow(COORD pos, int color)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (arr[i][j] == 1)
			{
				setColor(color, 15);
				ScreenPrint(pos.X + 2 * i, pos.Y + j, '  ', 2);
			}
		}
	}
}

void DrawRightArrow(COORD pos, int color)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			gotoXY(pos.X + 8 - 2 * i, pos.Y + j);
			if (arr[i][j] == 0)
			{
				setColor(color_black, 15);
			}
			if (arr[i][j] == 1)
			{
				setColor(color, 15);
				printf("  ");
			}
		}
	}
}

void ScreenDrawRightArrow(COORD pos, int color)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (arr[i][j] == 1)
			{
				setColor(color, 15);
				ScreenPrint(pos.X + 8 - 2 * i, pos.Y + j, '  ', 2);
			}
		}
	}
}

void DrawDownArrow(COORD pos, int color)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			gotoXY(pos.X + 2 * j, pos.Y + 4 - i);
			if (arr[i][j] == 0)
			{
				setColor(color_black, 15);
			}
			if (arr[i][j] == 1)
			{
				setColor(color, 15);
				printf("  ");
			}
		}
	}
}

void ScreenDrawDownArrow(COORD pos, int color)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			setColor(color_black, 15);
			if (arr[i][j] == 1)
			{
				setColor(color, 15);
				ScreenPrint(pos.X + 2 * j, pos.Y + 4 - i, '  ', 2);
			}
		}
	}
}


void ScreenDrawArrow(int keyNum, COORD pos)
{
	if (keyNum == LEFT)
	{
		ScreenDrawLeftArrow(pos, color_blue);
	}
	if (keyNum == RIGHT)
	{
		ScreenDrawRightArrow(pos, color_dark_yellow);
	}
	if (keyNum == UP)
	{
		ScreenDrawUpArrow(pos, color_red);
	}
	if (keyNum == DOWN)
	{
		ScreenDrawDownArrow(pos, color_green);
	}
}


/// 
/// �迭�� �������
/// 0 �̸� �ѱ�, 1�̸� ������ ����ϰ� ��� ���� ����
/// �������̽� �� Ű�� �׸���! Ű�� �ԷµǸ� ������ Ȱ��ȭ ��
void ScreenDrawKeyInterface()
{
	// �������̽� ��� ��ǥ ���� 
	// 5x5 ũ���� ȭ��ǥ. ����� ������ ���� ��� ��
	const int x = 8;
	const int y = 3;

	const int padding = 16;

	const COORD posLeft = { x + padding * 0, y };	// 8
	const COORD posDown = { x + padding * 1, y };	// 24
	const COORD posUp = { x + padding * 2, y };		// 40
	const COORD posRight = { x + padding * 3, y };	// 56

	/// �� ���� �⺻ ���´� ���
	// Up
	int colorUp = color_white;
	// Down
	int colorDown = color_white;
	// Left
	int colorLeft = color_white;
	// Right
	int colorRight = color_white;


	// Ű�� ������?
	if (GetKeyTable(UP))
	{
		// �ѹ��� �Է¹ޱ� ���� false ó��
		//SetKeyTable(UP, false);
		// ������ �������� �ٲ۴�
		colorUp = color_gray;
	}
	else
	{
		colorUp = color_white;
	}

	if (GetKeyTable(DOWN))
	{
		//SetKeyTable(DOWN, false);
		// ������ green���� �ٲ۴�
		colorDown = color_gray;
	}

	if (GetKeyTable(LEFT))
	{
		//SetKeyTable(LEFT, false);
		// ������ skyblue���� �ٲ۴�
		colorLeft = color_gray;
	}

	if (GetKeyTable(RIGHT))
	{
		//SetKeyTable(RIGHT, false);
		// ������ yellow���� �ٲ۴�
		colorRight = color_gray;
	}

	ScreenDrawUpArrow(posUp, colorUp);
	ScreenDrawRightArrow(posRight, colorRight);
	ScreenDrawDownArrow(posDown, colorDown);
	ScreenDrawLeftArrow(posLeft, colorLeft);
}





///
/// ��Ʈ ���, �̵�

///
/// ������ �ð����� ������Ʈ�� �ϵ��� ����
/// deltaTime 
///


// ���� �ð�
double startTime;
// ���� �ð�
double previousTime;
// ���� �ð�
double currentTime;
// �����ð� - ����ð�
double deltaTime;


const double runningSpeed = 75.f;		// 0.01 ��
const ULONGLONG BPM = 120;				// ���� 120 bpm �����̸�?
const ULONGLONG noteSpeed = 100;

int g_countU = 0;


/// <summary>
/// �ð� �ʱ�ȭ
/// </summary>
void InitTime()
{
	// ��ŸŸ�� �ʱ�ȭ = 0 (�����ð� 0)
	//startTime = currentTime = previousTime = GetTickCount64();
	// 1ms ������ ��ȯ

	startTime = currentTime = previousTime = (double)timeGetTime();
}

void UpdateTime()
{
	//currentTime = GetTickCount64();

	currentTime = (double)timeGetTime();

	deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	//g_ms_ForDebug_OriginalElaspedTime[g_countU] = deltaTime;
	//g_countU++;
}


// �ƽ�Ű ��Ʈ ���� 3
char** asciiArt[5];
//char** copyAnim[5];


/// ���� - ��Ŭ�� (�������� ��� �κ�) �� ���� ����
void UpdateRender_Song2()
{
	int anim1_frame = 7;
	int anim2_frame = 10;

	g_elapsedTimeB += deltaTime;
	
	// �ִϸ��̼� ������ ���� ����
	static int i = 0;
	static int j = 0;

	// ī���ÿ� ����ƽ
	static int _countB = 0;
	
	if (g_elapsedTimeB >= runningSpeed)
	{
		ScreenClear();

		// ���� ����� ������ �ۼ��Ѵ�
		ScreenDrawKeyInterface();
		GameLogic(1, 61000, num_hitobjects2);

		FindAsciiArt(asciiArtFilePath1, 10, 0);
		FindAsciiArt(asciiArtFilePath2, 7, 2);
		PrintAsciiArt(asciiArt[2], i, anim1_frame, 120, 15, color_dark_white);
		PrintAsciiArt(asciiArt[0], j, anim2_frame, 90, 13, color_dark_white);
		PrintCombo(100, 4, color_yellow);

		// �� �� ���۸� �����´�
		ScreenFlipping();

		g_elapsedTimeB -= runningSpeed;
		i++;
		j++;
	}
	if (i == anim1_frame)
	{
		i = 0;
	}
	if (j == anim2_frame)
	{
		j = 0;
	}
	
}



/// ���� - �������� ���� (�������� ��� �κ�) �� ���� ����
void UpdateRender_Song1()
{
	int anim1_frame = 7;

	g_elapsedTimeA += deltaTime;

	// �ִ� ������ ���� ����
	static int i = 0;

	// ī���ÿ� ����ƽ
	static int _countA = 0;

	if (g_elapsedTimeA >= runningSpeed)
	{
		// ���� ��� ������ �����
		ScreenClear();

		// ���� ����� ������ �ۼ��Ѵ�
		ScreenDrawKeyInterface();
		GameLogic(0, 91800, num_hitobjects);

		FindAsciiArt("bunny_", anim1_frame, 3);
		PrintAsciiArt(asciiArt[3], i, anim1_frame, 100, 12, color_white);
		PrintCombo(100, 4, color_yellow);

		// �� �� ���۸� �����´�
		ScreenFlipping();

		g_elapsedTimeA -= runningSpeed;
		i++;

	}
	if (i == anim1_frame)
	{
		i = 0;
	}

}


/// ���� - ����� ?
void UpdateRender_Song3()
{
	int anim1_frame = 8;

	g_elapsedTimeA += deltaTime;

	// �ִ� ������ ���� ����
	static int i = 0;

	// ī���ÿ� ����ƽ
	static int _countA = 0;

	if (g_elapsedTimeA >= runningSpeed)
	{
		// ���� ��� ������ �����
		ScreenClear();

		// ���� ����� ������ �ۼ��Ѵ�
		ScreenDrawKeyInterface();
		GameLogic(2, 131500, num_hitobjects3);

		FindAsciiArt("DeLaRue_", anim1_frame, 4);
		PrintAsciiArt(asciiArt[4], i, anim1_frame, 100, 14, color_white);
		PrintCombo(100, 4, color_yellow);

		// �� �� ���۸� �����´�
		ScreenFlipping();

		g_elapsedTimeA -= runningSpeed;
		i++;

	}
	if (i == anim1_frame)
	{
		i = 0;
	}

}



///
/// �ִϸ��̼� ���
/// 

// ���� ���, �ش� �ִϸ��̼��� ����ϴµ��� �ʿ��� ������ ����, ���° �ƽ�Ű ��Ʈ����
void FindAsciiArt(const char* asciiArtFilePath, int fileNum, int asciiNum)
{
	FILE* fp;
	char buffer[256];
	int line = 0;
	int index = 0;
	asciiArt[asciiNum] = (char**)malloc(sizeof(char*) * fileNum);


	// n ���� �ƽ�Ű ��Ʈ ���� �б�
	for (int i = 0; i < fileNum; i++)
	{
		asciiArt[asciiNum][i] = (char*)malloc(sizeof(char) * 5000);
		snprintf(buffer, 256, "%s%d.txt", asciiArtFilePath, i);
		errno_t err = fopen_s(&fp, buffer, "r");
		if (err != 0) {
			//printf("�ƽ�Ű ��Ʈ ������ ã�� �� �����ϴ�.");
			exit(1);
		}
		int readSize = fread(asciiArt[asciiNum][i], sizeof(char), 5000, fp);
		asciiArt[asciiNum][i][readSize] = '\0'; // �������� �� ���� �߰� > �̰� �� ���� �ϳ� ���� �ƽ�Ű ��Ʈ ���� ����
		fclose(fp);
	}
}

size_t starlen(const char* str)
{
	const char* s;
	for (s = str; *s; ++s);
	return(s - str);
}


// ���ۿ� ����� �ƽ�Ű ��Ʈ�� ����Ѵ� , �ƽ�Ű ��Ʈ�� ����, ���� ���� ���� ����, ���� ����, x,y ��ǥ
void PrintAsciiArt(char** asciiArt, int i, int n, int posx, int posy, int textcolor)
{

	int x = posx;
	int y = posy;   // ���� ��� ��ġ ����


	// i = 0 ���� i = 3 �� ������ �ε����� �ǹ�
	//for (int i = 0; i < 1; i++)
	{
		int line = 0;
		int index = 0;
		
		
		// while ���� readSize �� �о�� �ƽ�Ű ���ڼ��� �������� �߰��� �ι��ڸ� ������ ������ ������
		while (asciiArt[i][index] != '\0')
		{
			setColor(color_black, textcolor);
			ScreenPrint(x , y, asciiArt[i][index++], 1);
			x++;
			if (asciiArt[i][index] == '\n')
			{
				x = posx;
				index++;
				y++;   // ���� �ٷ� �̵��� ������ ����
				ScreenPrint(x, y, NULL, 0);
				
			}	
		}
	}

	// �����Ҵ� ����
	for (int i = 0; i < n; i++)
	{
		free(asciiArt[i]);
	}
	free(asciiArt);

}

char* copy_temp[4];

/// <summary>
/// �����Ҵ� �޸� �����ϴ� �Լ�
/// Ȥ�ó� ���������� ������ �ǳ�
/// ���̷���
/// </summary>
/// <param name="str"></param>
/// <param name="index"></param>
void Copy(const char* str, int index)
{
	char* print_temp;
	FILE* rfp;
	errno_t err = fopen_s(&rfp, str, "rt");
	print_temp = (char*)malloc(sizeof(char) * 3000);
	copy_temp[index] = (char*)malloc(sizeof(char) * 3000);


	// a �� �ִ� ���� b�� �����Ѵ�
	//memcpy(b, a, n * sizeof(int));
	memcpy(copy_temp[index], print_temp, sizeof(print_temp));

	if (err != 0)
	{
		//printf("���� �ҷ����⿡ �����߽��ϴ�.\n");
		exit(1);
	}

	// ������ ���ڿ� �ι��� �߰�
	int readSize = fread(copy_temp[index], sizeof(char), 3000, rfp);
	copy_temp[index][readSize] = '\0';
	fclose(rfp);

	free(print_temp);

}






///
/// 
/// Ÿ��Ʋ �޴� ��� �Լ�
/// ��ǥ�� ���� (10,2)

// Ÿ��Ʋ �ƽ�Ű�� ����ϴ� �Լ��Դϴ�.
void DrawTitle(int posx, int posy)
{
	int x = posx;
	int y = posy;
	static int color = color_blue;
	int index = 0;

	FILE* fp2;
	char bufferf[5000];
	snprintf(bufferf, 5000, "FMR_title.txt");
	errno_t err = fopen_s(&fp2, bufferf, "r");
	if (err != 0) {
		//printf("�ƽ�Ű ��Ʈ ������ ã�� �� �����ϴ�.");
		exit(1);
	}
	int readSize = fread(bufferf, sizeof(char), 5000, fp2);

	bufferf[readSize] = '\0';
	fclose(fp2);

	color = (color + 1) % 12 + 1;
	while (bufferf[index] != '\0')
	{
		color = (color + 1) % 12 + 1;
		setColor(color_black, color);

		ScreenPrint(x, y, bufferf[index++], 1);
		x++;
		if (bufferf[index] == '\n')
		{
			x = posx;
			//index++;
			y++;   // ���� �ٷ� �̵��� ������ ����
			//setColor(color_black, color);
			//ScreenPrint(x, y, NULL, 0);
		}
	}
}




/// 
/// y=32 ~ 40, ��� �÷� ����
/// �ƹ�Ű�� ���� ����.. ������ �ִϸ��̼����� ����� �ʹ�
void DrawSubtitle(int posx, int posy, int printcolor, int backcolor)
{
	// ������ ������
	int x = posx;
	int y = posy;

	// �۾�����
	int x2 = posx + 15;
	int y2 = posy + 2;


	static ULONGLONG elapsedTime_sub;
	elapsedTime_sub += deltaTime;


	char print_temp[5000];
	FILE* rfp;
	snprintf(print_temp, 5000, "subTitle2.txt");
	errno_t err = fopen_s(&rfp, print_temp, "rt");
	


	int index = 0;


	if (err != 0)
	{
		//printf("���� �ҷ����⿡ �����߽��ϴ�.\n");
		exit(1);
	}

	// ������ ���ڿ� �ι��� �߰�
	int readSize = fread(print_temp, sizeof(char), 5000, rfp);
	print_temp[readSize] = '\0';
	fclose(rfp);

	// ���ڸ� ���ڰŸ��� �ϰ�ʹ�

	// ���� ���� ����
	static bool setPrint = true;
	
	if (setPrint == true)
	{
		while (print_temp[index] != '\0')
		{
			// # ���ڸ� �������� ����ϴ°� �̻۰Ű���
			char c = print_temp[index];

			// # �ΰ��
			if (c == '#')
			{
				setColor(printcolor, color_dark_yellow);
				ScreenPrint(x2, y2, ' ', 1);
				//ScreenDraw(x2, y2, "#");
				x2++;
			}
			//setColor(color_gray, color_black);

			// �ٹٲ��̸� y �� ����, x ���� ���� ���������� �̵�
			if (c == '\n')
			{
				x2 = posx + 12;
				y2++;   // ���� �ٷ� �̵��� ������ ����
				setColor(backcolor, color_black);
				ScreenPrint(x2, y2, ' ', 1);
			}

			// ���� ������ ������ ������ ���ٲ㼭 �ƹ��͵� �Ⱥ��̰�
			if (c == ' ')
			{
				setColor(backcolor, color_black);
				ScreenPrint(x2, y2, c, 1);
				x2++;
			}
			index++;
		}


	}
	// ���� 1 �� �������� �Ⱥ��� ����
	if (elapsedTime_sub >= 1000)
	{
		setPrint = false;
		elapsedTime_sub = 0;
	}
	else
	{
		setPrint = true;
	}
}


/// <summary>
/// Ÿ��Ʋ�� ��µ� ��� �Լ��� ������۸� ����
/// </summary>
void TitleRender(bool isPlaying)
{
	if (isPlaying == false)
	{
		return;
	}


	static ULONGLONG elapsedTime_Title;
	elapsedTime_Title += deltaTime;

	if (elapsedTime_Title >= runningSpeed)
	{
		ScreenClear();

		DrawTitle(20, 3);
		DrawSubtitle(0, 35, color_dark_yellow, color_black);

		ScreenFlipping();

		elapsedTime_Title = 0;
	}
}



///
/// �� ���� �޴��� �׸���
/// 
/// �ش� ������ Ŀ���� �̵��ϸ�
/// 1. �����ִ� �� � ���� �ƽ�Ű ��Ʈ �ִϸ��̼��� �����̰�
/// 2. �ش� ���� �÷��̵ǰ�..!
/// ���Ժ��� ��Ŭ�� �ϵ��� �ϳ� ��
/// 


int GetSelectedMenu()
{
	// 1~3 �� ���� � ������ Ȯ���ϴ� ����
	static int menunumber = 1;

	// �Է� ���� ó���ϴ� �κ�..
	{

		if (menunumber < 1)
		{
			menunumber = 3;
		}
		if (menunumber > 3)
		{
			menunumber = 1;
		}


		if (GetKeyTable(RIGHT))
		{
			SetKeyTable(RIGHT, false);
			FMOD_Channel_Stop(channel2);
			bPlaySound = true;
			menunumber++;
		}
		if (GetKeyTable(LEFT))
		{
			SetKeyTable(LEFT, false);
			FMOD_Channel_Stop(channel2);
			bPlaySound = true;
			menunumber--;
		}
		if (GetKeyTable(SPACE))
		{
			//SetKeyTable(SPACE, false);
			return menunumber;
		}


	}

	// ���⼭ ������� �����ߴ°��� �޾ư���
	return menunumber;
}







/// ���� �ش� �޴��ٿ� Ŀ���� ���ٴ�� ���ڻ��� �ٲ� + (�뷡���� ����)
void DrawMenuList(int menunumber)
{

	static int menucolor1 = color_white;
	static int menucolor2 = color_white;
	static int menucolor3 = color_white;


	if (menunumber == 1)
	{
		menucolor1 = color_dark_yellow;
	}
	else
	{
		menucolor1 = color_white;

	}
	if (menunumber == 2)
	{
		menucolor2 = color_dark_yellow;
	}
	else
	{
		menucolor2 = color_white;

	}
	if (menunumber == 3)
	{
		menucolor3 = color_dark_yellow;

	}
	else
	{
		menucolor3 = color_white;

	}
	
	setColor(color_black, menucolor1);
	ScreenDraw(8, 38, "< H Y P E  B O Y > - N E W  J E A N S");
	setColor(color_black, menucolor1);
	ScreenDraw(23, 40, " E A S Y ");

	setColor(color_black, menucolor2);
	ScreenDraw(58, 38, "< �� �� �� ��  �� �� �� �� ? > - �� �� �� ��");
	setColor(color_black, menucolor2);
	ScreenDraw(73, 40, " N O R M A L ");

	setColor(color_black, menucolor3);
	ScreenDraw(110, 38, "< A  S i t e  D e  L a  R u e > - E Z 2 A C");
	setColor(color_black, menucolor3);
	ScreenDraw(127, 40, " H A R D ");

}





///
/// � �� �����ߴ��� ���ϰ����� ��ȯ
void DrawMenu()
{

	static ULONGLONG elapsedTime_menu;
	elapsedTime_menu += deltaTime;
	
	int menunumber = GetSelectedMenu();


	if (elapsedTime_menu >= runningSpeed)
	{

		ScreenClear();

		GetSelectedMenu();
		PlayAnim1(menunumber, 8, 3);
		PlayAnim2(menunumber, 63, 6);
		PlayAnim3(menunumber, 110, 7);
		DrawMenuList(menunumber);


		ScreenFlipping();


		elapsedTime_menu -= runningSpeed;
	}
}






///
/// ��Ŭ�� �ִ� ���
/// bool ������ �ִϸ� ����ϴ°��� ���� ����, x y �� ��ǥ
void PlayAnim2(int menunumber, int posx, int posy)
{
	int anim_frame = 7;

	static int i = 0;

	FindAsciiArt("ascii_art_ ", anim_frame, 0);
	

	if (menunumber == 2)
	{
		PrintAsciiArt(asciiArt[0], i, anim_frame, posx, posy, color_yellow);
		if (bPlaySound == true)
		{
			AudioPlay(fanclub, &channel2);
			bPlaySound = false;
		}
		//CopyPrintAsciiArt(0, anim_frame, posx, posy, color_yellow);
		i++;
	}
	else
	{
		PrintAsciiArt(asciiArt[0], i, anim_frame, posx, posy, color_dark_white);
	}

	if (i == anim_frame)
	{
		i = 0;
	}

}


/// ����
void PlayAnim1(int menunumber, int posx, int posy)
{
	int anim_frame = 7;

	static int i = 0;

	FindAsciiArt("bunny_", anim_frame, 1);


	if (menunumber == 1)
	{
		PrintAsciiArt(asciiArt[1], i, anim_frame, posx, posy, color_yellow);
		if (bPlaySound == true)
		{
			AudioPlay(hypeboy, &channel2);
			bPlaySound = false;
		}
		//CopyPrintAsciiArt(2, anim_frame, posx, posy, color_yellow);
		i++;
	}
	else
	{
		
		PrintAsciiArt(asciiArt[1], i, anim_frame, posx, posy, color_white);
	}

	if (i == anim_frame)
	{
		i = 0;
	}

}


void PlayAnim3(int menunumber, int posx, int posy)
{
	int anim_frame = 8;

	static int i = 0;

	FindAsciiArt("DeLaRue_", anim_frame, 4);

	if (menunumber == 3)
	{
		PrintAsciiArt(asciiArt[4], i, anim_frame, posx, posy, color_yellow);
		if (bPlaySound == true)
		{
			AudioPlay(delarue, &channel2);
			bPlaySound = false;
		}
		i++;
	}
	else
	{
		PrintAsciiArt(asciiArt[4], i, anim_frame, posx, posy, color_white);
	}

	if (i == anim_frame)
	{
		i = 0;
	}
}

















///
/// 04.20
/// ���� �Լ�
/// y ���� Ư�� ������ �Է��� �޾Ҵ°�,, �� �����Ѵ�
/// ���� ���� ���� �ִ� ���� �޺��� �ö󰡰�..
/// ���� ���� �ۿ� �ִ� ���� �̽� ī��Ʈ�� �ö󰣴�..
/// ���� bool ������ ������ �ʿ䰡 �ִ�
/// is Good, is Miss
/// 


/// ���� ���� ���� ���߿� ��Ʈ ������ �����ֱ�
// 4 �� �¿���� Ű ����, 100 �� ��Ʈ ����
bool isGood[4][2000];
bool isMiss[4][100];

/// <summary>
/// ����
/// </summary>
/// <param name="y">������ ������ �Ǵ� y���� ��ġ</param>
/// <param name="i">��Ʈ �迭�� index</param>
/// <param name="key">�����¿� �� ���� Ű����</param>
/// <returns>isGood �� ���� ��ȯ��</returns>

void HitBox3(HitObject obj, int i)
{
	// ����� �÷���Ÿ���� �����ϴ� ����
	unsigned _hitElapsedTime = GetAudioPlaybackTime(channel2);
	int key;

	if (obj.x == 64)
		key = LEFT;
	else if (obj.x == 192)
		key = DOWN;
	else if (obj.x == 320)
		key = UP;
	else if (obj.x == 448)
		key = RIGHT;

	if (obj.time + 200 > _hitElapsedTime && obj.time - 100 < _hitElapsedTime)
	{
		if (GetKeyTable(key) && !isGood[key][i])
		{
			isGood[key][i] = true;
			setColor(color_green, color_red);
			ScreenPrint(100, 2, 'DOOG', 4);
			combo++;

			SetKeyTable(key, false);
		}
		else
		{
			SetKeyTable(key, false);
		}
	}
	else if (obj.time + 200 <= _hitElapsedTime && !isGood[key][i] && !isMiss[key][i])
	{
		AudioPlay(tick, &channel);

		missCount++;
		combo = 0;
		setColor(color_red, color_green);
		ScreenPrint(100, 3, 'SSiM', 4);
		isMiss[key][i] = true;
		isGood[key][i] = false;
		SetKeyTable(LEFT, false);
		SetKeyTable(RIGHT, false);
		SetKeyTable(UP, false);
		SetKeyTable(DOWN, false);
	}

	


	/// Ȯ�ο�
	char convertMiss[20] = { 0 };
	char convertCombo[20] = { 0 };

	snprintf(convertCombo, sizeof(convertCombo), "%d", combo);
	const char* constCombo = convertCombo;


	snprintf(convertMiss, sizeof(convertMiss), "%d", missCount);
	const char* constMiss = convertMiss;
	setColor(color_yellow, color_blue);
}




///
/// 04.21
/// hp ������ & ���� ���
/// ������ 100000 �ʸ���

void PrintScore(int posx, int posy, int color)
{
	int score = 0;
	score = 999999 - missCount * 34600;

	int c[6];
	int x = posx;
	int y = posy;


	if (score <= 0)
	{
		x = posx - 45;
		y = posy - 5;

		FILE* fp2;
		char bufferf[3000];
		snprintf(bufferf, 3000, "gameover1.txt");
		errno_t err = fopen_s(&fp2, bufferf, "r");
		if (err != 0) {
			//printf("�ƽ�Ű ��Ʈ ������ ã�� �� �����ϴ�.");
			exit(1);
		}
		int readSize = fread(bufferf, sizeof(char), 3000, fp2);

		bufferf[readSize] = '\0';
		fclose(fp2);

		int n = 0;

		while (bufferf[n] != '\0')
		{
			char o = bufferf[n];

			if (bufferf[n] == '\n')
			{
				x = posx - 45;
				y++;   // ���� �ٷ� �̵��� ������ ����
			}

			else if (o != ' ')
			{
				setColor(color_dark_red, color_dark_red);

				ScreenPrint(x, y, o, 1);
				x++;
			}

			if (o == ' ')
			{
				setColor(color_black, color_black);
				ScreenPrint(x, y, ' ', 1);
				x++;
			}

			
			n++;
		}
		return;
	}

		
	
	
	{
		c[0] = score / 100000;
		c[1] = (score % 100000) / 10000;
		c[2] = (score % 10000) / 1000;
		c[3] = (score % 1000) / 100;
		c[4] = (score % 100) / 10;
		c[5] = (score % 10) / 1;
	}


	
	FILE* fp3;
	char bufferff[3000];
	snprintf(bufferff, 3000, "score.txt");
	errno_t err = fopen_s(&fp3, bufferff, "r");
	if (err != 0) {
		//printf("�ƽ�Ű ��Ʈ ������ ã�� �� �����ϴ�.");
		exit(1);
	}
	int readSize = fread(bufferff, sizeof(char), 3000, fp3);

	bufferff[readSize] = '\0';
	fclose(fp3);

	int n = 0;

	
	y = posy - 10;
	x = posx + 3;

	while (bufferff[n] != '\0')
	{
		char o = bufferff[n];
		

		if (o == '#')
		{
			setColor(color_purple, color_purple);

			ScreenPrint(x, y, ' ', 1);
			x++;
		}

		if (o == ' ')
		{
			setColor(color_black, color_black);
			ScreenPrint(x, y, ' ', 1);
			x++;
		}

		if (o == '\n')
		{
			x = posx + 3;
			y++;   // ���� �ٷ� �̵��� ������ ����
		}
		n++;
	}


	FILE* fp[6];
	
	char buffer_forN[6][50];
	char buffer_for9[50];
	



	for (int i = 0; i < 6; i++)
	{
		int n = 0;

		snprintf(buffer_forN[i], 50, "%s%d.txt", "num_", c[i]);
		errno_t err = fopen_s(&fp[i], buffer_forN[i], "r");
		if (err != 0) {
			//printf("�ƽ�Ű ��Ʈ ������ ã�� �� �����ϴ�.");
			exit(1);
		}
		int readSize = fread(buffer_forN[i], sizeof(char), 50, fp[i]);
	
		buffer_forN[i][readSize] = '\0';
		fclose(fp[i]);

		
		x = posx + i * 8 + 2;
		y = posy + 3;

		while (buffer_forN[i][n] != '\0')
		{
			char o = buffer_forN[i][n];

			if (o == '#')
			{
				setColor(color, color);

				ScreenPrint(x, y, ' ', 1);
				x++;
			}
			
			if (o == ' ')
			{
				setColor(color_black, color_black);
				ScreenPrint(x, y, ' ', 1);
				x++;
			}

			if (buffer_forN[i][n] == '\n')
			{
				x = posx + i * 8 + 2;
				y++;   // ���� �ٷ� �̵��� ������ ����
			}
			n++;
		}
	}

}

int scoreDelay = 0;

void RenderScore()
{
	static ULONGLONG scoreElapsedTime;
	scoreElapsedTime += deltaTime;

	if (scoreElapsedTime >= runningSpeed)
	{
		ScreenClear();

		PrintScore(55, 25, color_white);

		ScreenFlipping();

		scoreDelay++;

		if (scoreDelay == 50)
		{
			bPlaySound = true;
			selectedNum = 0;
		}

		scoreElapsedTime -= runningSpeed;
	}

}






/// <summary>
/// 04.26 �Լ� ��ġ��
/// GetAudioPlaybackTime(channel2) �� �ν��� �� �ȵǴ� Ȯ���غ���
/// </summary>
void ParcingNote()
{
	/// �������
	yHeight -= judgeLineY;
	ULONGLONG startTime = GetTickCount64();

	//AudioSystem();
	AudioCreate("Assets/maintheme.mp3", &maintheme);
	AudioCreate("Assets/fanclub.mp3", &fanclub);
	AudioCreate("Assets/Kamui.mp3", &kamui);
	AudioCreate("Assets/hypeboy.mp3", &hypeboy);
	AudioCreate("Assets/tick.wav", &tick);
	AudioCreate("Assets/start.wav", &start);
	AudioCreate("Assets/DeLaRue.mp3", &delarue);
	AudioCreate("Assets/clear.mp3", &clear);
	AudioCreate("Assets/gameover.mp3", &gameover);
	
	//AudioPlay(kamui, &channel2);
	//AudioPlay(hypeboy, &channel2);

	fp1 = fopen(filename_hypeboy, "r");
	if (fp1 == NULL) {
		printf("������ �ҷ��� �� �����ϴ�. %s\n", filename_hypeboy);
		exit(1);
	}

	// �� hit object line�� �Ľ� 
	while (fgets(buffer, sizeof(buffer), fp1)) {
		if (sscanf(buffer, "%d,%d,%d,%d,%d,%*d:%*d:%*d:%*d:\n",
			&hitobjects[0][num_hitobjects].x,
			&hitobjects[0][num_hitobjects].y,
			&hitobjects[0][num_hitobjects].time,
			&hitobjects[0][num_hitobjects].type,
			&hitobjects[0][num_hitobjects].hit_sound) != 5) 
		{
			break;
		}
		num_hitobjects++;
	}

	fclose(fp1);


	
	fp2 = fopen(filename_fanclub, "r");
	if (fp2 == NULL) {
		printf("������ �ҷ��� �� �����ϴ�. %s\n", filename_fanclub);
		exit(1);
	}

	// �� hit object line�� �Ľ� 
	while (fgets(buffer2, sizeof(buffer2), fp2)) {
		if (sscanf(buffer2, "%d,%d,%d,%d,%d,%*d:%*d:%*d:%*d:\n",
			&hitobjects[1][num_hitobjects2].x,
			&hitobjects[1][num_hitobjects2].y,
			&hitobjects[1][num_hitobjects2].time,
			&hitobjects[1][num_hitobjects2].type,
			&hitobjects[1][num_hitobjects2].hit_sound) != 5) {
			break;
		}
		num_hitobjects2++;
	}

	fclose(fp2);


	fp3 = fopen(filename_delarue, "r");
	if (fp3 == NULL) {
		printf("������ �ҷ��� �� �����ϴ�. %s\n", filename_delarue);
		exit(1);
	}

	// �� hit object line�� �Ľ� 
	while (fgets(buffer3, sizeof(buffer3), fp3)) {
		if (sscanf(buffer3, "%d,%d,%d,%d,%d,%*d:%*d:%*d:%*d:\n",
			&hitobjects[2][num_hitobjects3].x,
			&hitobjects[2][num_hitobjects3].y,
			&hitobjects[2][num_hitobjects3].time,
			&hitobjects[2][num_hitobjects3].type,
			&hitobjects[2][num_hitobjects3].hit_sound) != 5) {
			break;
		}
		num_hitobjects3++;
	}

	fclose(fp3);


	/// ������� �߰���
}



bool isExitGame = false;

/// <summary>
/// ����ؾ� �ϴ°� ���� �ִ� ����
/// num �� �� ���� ��� ������, ��Ÿ���� �뷡 �ѽð�, �迭 ��
/// </summary>
void GameLogic(int num, unsigned run_time, int number_hitobjects)
{
	const int x = 8;
	const int padding = 16;


	// ����� �÷���Ÿ���� �����ϴ� ����
	unsigned elapsedTime = GetAudioPlaybackTime(channel2);
	if (elapsedTime >= run_time)
	{
		AudioPlay(clear, &channel);
		FMOD_Channel_Stop(channel2);
		isExitGame = true;
	}
	if (missCount >= 20)
	{
		// Ÿ��Ʋ ���� �ߴ�
		AudioPlay(gameover, &channel);
		FMOD_Channel_Stop(channel2);
		isExitGame = true;
		missCount = 1000;
	}

	for (int i = startNote; i < number_hitobjects; ++i)
	{
		COORD posLeft = { x + padding * 0, hitobjects[num][i].y };	// 8
		COORD posDown = { x + padding * 1, hitobjects[num][i].y };	// 24
		COORD posUp = { x + padding * 2, hitobjects[num][i].y };		// 40
		COORD posRight = { x + padding * 3, hitobjects[num][i].y };	// 56

		// hitobject.time �� �� ��ü�� �����Ǿ���ϴ� �ð� - �÷��� �ð�
		int isPass = hitobjects[num][i].time - elapsedTime;

		// �����Ǳ� ��, ������ �ʿ� ���� �ֵ��� Ž���� ������
		if (isPass >= scrollSpeed)
			break;

		// y ���� ��ȭ��
		hitobjects[num][i].y = (double)(hitobjects[num][i].time - elapsedTime) / scrollSpeed * (yHeight-3);

		HitBox3(hitobjects[num][i], i);
		
		if ((hitobjects[num][i].y > 1.f) && (hitobjects[num][i].y < 42.f))
		{
			if (hitobjects[num][i].x == 64)
			{
				ScreenDrawLeftArrow(posLeft, color_blue);
			}
			else if (hitobjects[num][i].x == 192)
			{
				ScreenDrawDownArrow(posDown, color_green);
			}
			else if (hitobjects[num][i].x == 320)
			{
				ScreenDrawUpArrow(posUp, color_red);
			}
			else if (hitobjects[num][i].x == 448)
			{
				ScreenDrawRightArrow(posRight, color_dark_yellow);
			}
		}
		
		// �̹� ������ �ֵ� (�� ��Ž���� �����ð��� ���� �ֵ�)�� ���ܽ�Ų��
		if (hitobjects[num][i].time + 2000 < elapsedTime)
		{
			++startNote;
		}
	}
}



void PrintCombo(int posx, int posy, int color)
{
	int c[3];

	c[0] = (combo % 1000) / 100;
	c[1] = (combo % 100) / 10;
	c[2] = (combo % 10) / 1;

	FILE* fp[3];

	char buffer_forN[3][50];

	int x = posx;
	int y = posy;

	for (int i = 0; i < 3; i++)
	{
		int n = 0;
		snprintf(buffer_forN[i], 50, "%s%d.txt", "num_", c[i]);
		errno_t err = fopen_s(&fp[i], buffer_forN[i], "r");
		if (err != 0) {
			//printf("�ƽ�Ű ��Ʈ ������ ã�� �� �����ϴ�.");
			exit(1);
		}
		int readSize = fread(buffer_forN[i], sizeof(char), 50, fp[i]);

		buffer_forN[i][readSize] = '\0';
		fclose(fp[i]);


		x = posx + i * 8;
		y = posy;

		while (buffer_forN[i][n] != '\0')
		{
			char o = buffer_forN[i][n];
			

			if (o == '#')
			{
				setColor(color, color);
				ScreenPrint(x, y, ' ', 1);
				x++;
			}

			if (o == ' ')
			{
				setColor(color_black, color_black);
				ScreenPrint(x, y, ' ', 1);
				x++;
			}

			if (buffer_forN[i][n] == '\n')
			{
				x = posx + i * 8;
				y++;   // ���� �ٷ� �̵��� ������ ����
			}
			n++;
		}
	}

}


void InitHitBox()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2000; j++)
		{
			isGood[i][j] = NULL;
		}
		for (int j = 0; j < 100; j++)
		{
			isMiss[i][j] = NULL;
		}
	}
}







void gLoop_1()
{
	UpdateTime();
	UpdateInput();
	UpdateRender_Song1();
}



void gLoop_2()
{
	UpdateTime();
	UpdateInput();
	UpdateRender_Song2();
}


void gLoop_3()
{
	UpdateTime();
	UpdateInput();
	UpdateRender_Song3();

}


void TitleLoop()
{
	UpdateTime();
	UpdateMenuInput();
	TitleRender(true);
}


void MenuLoop()
{
	UpdateTime();
	UpdateMenuInput();
	DrawMenu();
}


void ScoreLoop()
{
	UpdateTime();
	RenderScore();
	UpdateMenuInput();
}







void InitGame()
{
	/// �ʱ�ȭ
	// �ܼ� ����
	initConsole();
	// �ð� �ʱ�ȭ
	InitTime();

	// ����� �ý���
	AudioSystem();

	ParcingNote();


	// Ÿ��Ʋ�� ����� �ƽ�Ű ��Ʈ ����
	//Copy("FMR_title.txt", 0);
	Copy("subTitle2.txt", 1);
}




void GameManager()
{
	switch (selectedNum)
	{
		// Ÿ��Ʋ
	case 0:
		if (bPlaySound == true)
		{
			InitHitBox();
			AudioPlay(maintheme, &channel2);
			bPlaySound = false;
			missCount = 0;
			combo = 0;
			scoreDelay = 0;
		}
		TitleLoop();

		if (GetKeyTable(SPACE))
		{
			// Ÿ��Ʋ ���� �ߴ�
			AudioPlay(start, &channel);
			FMOD_Channel_Stop(channel2);
			SetKeyTable(SPACE, false);
			isExitGame = false;
			selectedNum = 4;
			bPlaySound = true;
		}
		break;

	case 1:
		//if (bPlaySound == true)
		{
			AudioPlay(hypeboy, &channel2);
			bPlaySound = false;
		}
		while (!isExitGame)
		{
			gLoop_1();
		}
		SetKeyTable(SPACE, false);
		selectedNum = 5;
		startNote = 0;
		break;

	case 2:
		//if (bPlaySound == true)
		{
			AudioPlay(fanclub, &channel2);
			bPlaySound = false;
		}
		while (!isExitGame)
		{
			gLoop_2();
		}
		SetKeyTable(SPACE, false);
		selectedNum = 5;
		startNote = 0;
		break;


	case 3:
		//if (bPlaySound == true)
		{
			AudioPlay(delarue, &channel2);
			bPlaySound = false;
		}
		while (!isExitGame)
		{
			gLoop_3();
		}
		SetKeyTable(SPACE, false);
		selectedNum = 5;
		startNote = 0;
		break;


		// �޴�
	case 4:
		if (GetKeyTable(SPACE))
		{
			AudioPlay(start, &channel);
			FMOD_Channel_Stop(channel2);
			selectedNum = GetSelectedMenu();
			SetKeyTable(SPACE, false);
		}
		MenuLoop();
		break;

		// ���ھ�
	case 5: 
		ScoreLoop();
		
		break;

	default:
		break;
	}

	

}



int main()
{
	/// �ʱ�ȭ
	InitGame();

	while (1)
	{
		GameManager();
	}
}