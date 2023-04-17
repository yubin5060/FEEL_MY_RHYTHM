#define MAX_NOTE 6
#define ROW_SIZE 180
#define COLUMN_SIZE 60
#include "input.h"

//using namespace std;



///
/// 04. 10
/// 커서 받기 gotoxy, initConsole 등
/// 창 사이즈 설정
/// ***더블 버퍼링 찾아보기***
/// 






// 화면 버퍼값 받아올 구조체 (사각형마냥 값을 저장한다고 함)
SMALL_RECT consoleScreenSize;
// UI 최대 사이즈 설정값
SMALL_RECT _UImaxSize;
// 버퍼(화면) 2개 설정
static HANDLE hScreen[2];
static int nScreenIndex;


/// <summary>
/// 초기화
/// 버퍼 받기 -> 나중에 콘솔창 크기 따로 설정하기
/// </summary>
void initConsole()
{
	// 더블 버퍼링.. 버퍼 두개 만들기
	hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	// 커서 숨겨버리기 (안깜빡임! 안보임!)
	HANDLE consonleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO consoleCursor = { 0, };
	consoleCursor.bVisible = 0;											// 0 이면 커서를 감춤, 0 이 아닐 경우에 보인다
	consoleCursor.dwSize = 1;											// 값에 따라 커서의 크기를 조절한다

	// 커서에대한 설정값을 각 화면에 입력
	SetConsoleCursorInfo(consonleHandle, &consoleCursor);
	SetConsoleCursorInfo(hScreen[0], &consoleCursor);
	SetConsoleCursorInfo(hScreen[1], &consoleCursor);


	// 화면 사이즈 받아오기 (버퍼값 받아서 ScreenSize 에 대입)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	consoleScreenSize.Left = csbi.srWindow.Left;
	consoleScreenSize.Right = csbi.srWindow.Right;
	consoleScreenSize.Bottom = csbi.srWindow.Bottom;
	consoleScreenSize.Top = csbi.srWindow.Top;

	_UImaxSize.Left = consoleScreenSize.Left + 2;
	_UImaxSize.Right = consoleScreenSize.Right - 2;
	_UImaxSize.Bottom = consoleScreenSize.Bottom - 2;
	_UImaxSize.Top = consoleScreenSize.Top + 2;

	// 화면 초기화
	//system("cls");


}



/// 버퍼 두개를 전환해주는 함수
void ScreenFlipping()
{
	SetConsoleActiveScreenBuffer(hScreen[nScreenIndex]);
	nScreenIndex = !nScreenIndex;
}


/// 화면을 비우는 함수. 다음 버퍼로 전환할 때 이전 출력내용을 비워줘야한다
void ScreenClear()
{// [가로줄에 전부 공백 채우기]를 세로줄 개수만큼 돌림
	for (int i = 0; i < COLUMN_SIZE; i++)
	{
		COORD Coor = { 0, i };
		DWORD dw;
		// 공백을 화면크기만큼 입력해줘야함;;
		FillConsoleOutputCharacter(hScreen[nScreenIndex], ' ', ROW_SIZE, Coor, &dw);
	}
	
}


/// 버퍼에 그림그리기
void ScreenPrint(int x, int y, char* string)
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition(hScreen[nScreenIndex], CursorPosition);
	WriteFile(hScreen[nScreenIndex], string, strlen(string), &dw, NULL);
}



/// 버퍼 닫기
void ScreenRelease()
{
	CloseHandle(hScreen[0]);
	CloseHandle(hScreen[1]);
}


/// 렌더 (직접적인 출력 부분)
void Render()
{
	// 이전 출력 내용을 지운다
	ScreenClear();

	// 새로 출력할 내용을 작성한다


	// 앞 뒤 버퍼를 뒤집는다
	ScreenFlipping();

}





/// <summary>
/// 커서 위치 이동하는 함수
/// y 좌표는 아래로 갈수록 증가, 원점은 좌측 상단
/// </summary>
/// <param name="x">이동할 x좌표</param>
/// <param name="y">이동할 y좌표</param>
void gotoXY(int x, int y)
{
	COORD cursor = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
}











///
/// 04. 10
/// 인풋 값을 받으면
/// 색칠이 되는 UI
/// 거의 전체화면
/// 




/// <summary>
/// 색깔 팔레트
/// </summary>
const int color_black = 0;
const int color_white = 15;
const int color_green = 10;
const int color_red = 12;
const int color_yellow = 6;
const int color_dark_white = 7;
const int color_blue = 9;
const int color_gray = 8;


// 배경 색, 글꼴 색 지정
void setColor(int backColor, int textColor)
{
	textColor = color_white;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (backColor << 4) + textColor);
	// SetConsoleTextAttribute(hScreen[nScreenIndex], (backColor << 4) + textColor);
}




/// <summary>
/// 화살표 출력 원본 (UP KEY)
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
/// 너무 지저분하고 다양한 사용(나중에 위에 올라올 노트 출력에도 써야하니)
/// 따로 빼줘볼까
/// 상하좌우 화살표 출력 함수이다
/// </summary>
/// <param name="pos">좌표 정보를 담고있는 구조체</param>
/// <param name="color">무슨 색으로 출력할깡?</param>
void DrawUpArrow(COORD pos, int color)
{

	// arr[i][j] up
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			// j 는 가로 출력 (x), i 는 세로 출력을 담당 (y)
			// 가로는 세로보다 두배 출력해야함.. -> j*2
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

//void ScreenInfoSave(int n)
//{
//	memcpy(arr, )
//}


//void ScreenDrawUpArrow(COORD pos, int color)
//{
//	for (int i = 0; i < 5; i++)
//	{
//		for (int j = 0; j < 5; j++)
//		{
//			gotoXY(pos.X + 2 * j, pos.Y + i);
//			if (arr[i][j] == 0)
//			{
//				setColor(color_black, 15);
//			}
//			if (arr[i][j] == 1)
//			{
//				setColor(color, 15);
//				printf("  ");
//			}
//		}
//	}
//}

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






/// 
/// 배열로 출력하자
/// 0 이면 넘김, 1이면 공백을 출력하고 배경 색상 변경
/// 인터페이스 상 키를 그린다! 키가 입력되면 색상이 활성화 됨
void DrawKeyInterface()
{
	// 인터페이스 출력 좌표 지정 
	// 5x5 크기의 화살표. 출력의 시작은 좌측 상단 끝
	const int x = 8;
	const int y = 2;

	const int padding = 16;

	const COORD posLeft = { x + padding * 0, y };
	const COORD posDown = { x + padding * 1, y };
	const COORD posUp = { x + padding * 2, y };
	const COORD posRight = { x + padding * 3, y };

	/// 안 누른 기본 상태는 흰색
	// Up
	int colorUp = color_dark_white;
	// Down
	int colorDown = color_dark_white;
	// Left
	int colorLeft = color_dark_white;
	// Right
	int colorRight = color_dark_white;


	// 키가 눌리면?
	if (GetKeyTable(UP))
	{
		// 한번만 입력받기 위해 false 처리
		SetKeyTable(UP, false);
		// 색상을 빨강으로 바꾼다
		colorUp = color_red;
	}

	if (GetKeyTable(DOWN))
	{
		SetKeyTable(DOWN, false);
		// 색상을 green으로 바꾼다
		colorDown = color_green;
	}

	if (GetKeyTable(LEFT))
	{
		SetKeyTable(LEFT, false);
		// 색상을 skyblue으로 바꾼다
		colorLeft = color_blue;
	}

	if (GetKeyTable(RIGHT))
	{
		SetKeyTable(RIGHT, false);
		// 색상을 yellow으로 바꾼다
		colorRight = color_yellow;
	}

	DrawUpArrow(posUp, colorUp);
	DrawRightArrow(posRight, colorRight);
	DrawDownArrow(posDown, colorDown);
	DrawLeftArrow(posLeft, colorLeft);

	/// 다른 함수로 정리함
	{
		// 이차원 배열을 설정하고 화살표 모양을 만듦
	//int arr[5][5] =
	//{
	//	{0,0,1,0,0},
	//	{0,1,1,1,0},
	//	{1,0,1,0,1},
	//	{0,0,1,0,0},
	//	{0,0,1,0,0}
	//};

	// arr[i][j] up
	//for (int i = 0; i < 5; i++)
	//{
	//	for (int j = 0; j < 5; j++)
	//	{
	//		// j는 가로 출력 (x), i는 세로 출력을 담당 (y)
	//		// 가로는 세로보다 두배 출력해야함.. -> j*2
	//		gotoXY(x + 2 * j, y + i);
	//		//if (arr[i][j] == 0)
	//		//{
	//		//	setColor(color_black);
	//		//	//printf("  ");
	//		//}	 없어도 무관
	//		if (arr[i][j] == 1)
	//		{
	//			setColor(color);
	//			printf("  ");
	//		}
	//	}
	//}

	// arr[i][j] left
	// y=x 대칭 (윈도우의 y축은 아래쪽 증가)
	//for (int i = 0; i < 5; i++)
	//{
	//	for (int j = 0; j < 5; j++)
	//	{
	//		gotoXY(x +  2*i, y + j);
	//		
	//		if (arr[i][j] == 1)
	//		{
	//			setColor(color);
	//			printf("  ");
	//		}
	//	}
	//}


	// arr[i][j] right
	// left의 x= 5 대칭 (가로줄은 두칸단위임!!!!!) -> 그래서 (5-1)가 아니라 (10-2)
	//for (int i = 0; i < 5; i++)
	//{
	//	for (int j = 0; j < 5; j++)
	//	{
	//		gotoXY(x + 8 - 2 * i, y + j);
	//		
	//		if (arr[i][j] == 1)
	//		{
	//			setColor(color);
	//			printf("  ");
	//		}
	//	}
	//}
	//

	// arr[i][j] 
	// 배열은 항상 0번째부터 출력됨을 명심하자!! (5 - 1) = 4
	//for (int i = 0; i < 5; i++)
	//{
	//	for (int j = 0; j < 5; j++)
	//	{
	//		gotoXY(x + 2 * j, y + 4 - i);
	//		
	//		if (arr[i][j] == 1)
	//		{
	//			setColor(color);
	//			printf("  ");
	//		}
	//	}
	//}


	/*setColor(color_black);
	printf("    ");
	setColor(color, 15);
	printf("  \n");

	setColor(color_black);
	printf("  ");
	setColor(color, 15);
	printf("  ");
	setColor(color_black);
	printf("  ");
	setColor(color, 15);
	printf("  \n");

	setColor(color, 15);
	printf("  ");
	setColor(color_black);
	printf("      ");
	setColor(color, 15);
	printf("  ");

	setColor(color_black);*/
	}
}





void Interface()
{


}




///
/// 노트 출력, 이동

///
/// 일정한 시간마다 업데이트를 하도록 하자
/// deltaTime 
///

// 이전 시간
ULONGLONG previousTime;
// 현재 시간
ULONGLONG currentTime;
// 이전시간 - 현재시간
ULONGLONG deltaTime;

int updateCount;
int fixedUpdateCount;
const ULONGLONG noteSpeed = 50;
const ULONGLONG BPM = 100;

/// <summary>
/// 시간 초기화
/// </summary>
void InitTime()
{
	// 델타타임 = 0 (누적시간 0)
	currentTime = previousTime = GetTickCount64();
	// 1ms 단위로 반환
}


void UpdateTime()
{
	previousTime = currentTime;
	currentTime = GetTickCount64();

	deltaTime = currentTime - previousTime;
}

ULONGLONG GetDeltaTime()
{
	return deltaTime;								// 단위 : 1/1000 초
}


int l_note[] = { 1,0,1,0,1,1 };
COORD NotecurPos_l[] = { {8,30},{8,30},{8,30},{8,30},{8,30},{8,30} };
COORD NoteprePos_l[] = { {8,30},{8,30},{8,30},{8,30},{8,30},{8,30} };

///
/// y 축 한줄 씩 -1 시키기
/// _UIMaxSize.Bottom -1
/// L(8) D(24) U(40) R(56)
void UpdateNotePosition(int i)
{
	if (l_note[i] == 1)
	{
		NoteprePos_l[i] = NotecurPos_l[i];
		DrawLeftArrow(NoteprePos_l[i], color_black);
		NotecurPos_l[i].Y--;

		// 만약 화살표가 화면을 벗어나면 그만 출력시킨다
		// 판정 기능 구현 후
		// 판정 안에 타격하면 UImaxSize 에서 리턴, 미스는 콘솔사이즈에서 리턴
		if (NotecurPos_l[i].Y < consoleScreenSize.Top)
		{
			return;
		}
		DrawLeftArrow(NotecurPos_l[i], color_gray);
	}

	if (l_note[i] == 0)
		return;
}

void UpdateBPM()
{
	static ULONGLONG elapsedTime;
	elapsedTime += deltaTime;

	for (int i = 0; i < MAX_NOTE; i++)

	{
		if (elapsedTime >= BPM * i)
		{
			UpdateNotePosition(i);

		}
	}

}

void UpdateNote()
{
	static ULONGLONG elapsedTime;
	elapsedTime += deltaTime;

	if (elapsedTime >= noteSpeed)
	{
		UpdateBPM();
		elapsedTime -= noteSpeed;
	}


}
































int main()
{
	initConsole();
	InitTime();
	system("cls");


	while (1)
	{
		UpdateTime();
		UpdateInput();
		DrawKeyInterface();
		UpdateNote();
		ScreenClear();


		//system("cls");
	}
}