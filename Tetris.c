#define _CRT_NONSTDC_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define randomize() srand((unsigned)time(NULL))
#define random(n) (rand() % (n))
#define delay(n) Sleep(n)
#define clrscr() system("cls")
#define gotoxy(x,y) { COORD Cur = {x, y}; \
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),Cur);}
#define showcursor(bShow) { CONSOLE_CURSOR_INFO CurInfo = {20, bShow}; \
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&CurInfo); }
#define BH 20



enum { ESC = 27, LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80 };
#define putsxy(x, y, s) {gotoxy(x, y);puts(s);}
#define BX 5
#define BY 1
#define BW 20
#define BH 20

BOOL ProcessKey();
void PrintBrick(BOOL Show);
int GetAround(int x, int y, int b, int r);
BOOL MoveDown();
void TestFull();
void DrawScreen();

void write_file();
void read_file();
void InitializeGameTime();
int GetGameTime();
void color(int c);
void playtime();
void DrawScreenWithColors(int colorsToRemove[][BH + 2]);
int ControlSpeed(int score);

//블록 자체에 색상을 추가했어요
struct Point {
	int x, y;
	int color;
};

struct Point Shape[][4][4] = {
	{ { 0,0,1,1,0,1,2,0,1,-1,0,1 },{ 0,0,1,0,1,1,0,-1,1,0,-2,1 },{ 0,0,1,1,0,1,2,0,1,-1,0,1 },{ 0,0,1,0,1,1,0,-1,1,0,-2 ,1} },
	{ { 0,0,2,1,0,2,0,1,2,1,1,2 },{ 0,0,2,1,0,2,0,1,2,1,1,2 },{ 0,0,2,1,0,2,0,1,2,1,1,2 },{ 0,0,2,1,0,2,0,1,2,1,1,2 } },
	{ { 0,0,3,-1,0,3,0,-1,3,1,-1,3 },{ 0,0,3,0,1,3,-1,0,3,-1,-1,3 },{ 0,0,3,-1,0,3,0,-1,3,1,-1,3 },{ 0,0,3,0,1,3,-1,0,3,-1,-1,3 } },
	{ { 0,0,4,-1,-1,4,0,-1,4,1,0,4 },{ 0,0,4,-1,0,4,-1,1,4,0,-1,4 },{ 0,0,4,-1,-1,4,0,-1,4,1,0,4 },{ 0,0,4,-1,0,4,-1,1,4,0,-1,4 } },
	{ { 0,0,5,-1,0,5,1,0,5,-1,-1,5 },{ 0,0,5,0,-1,5,0,1,5,-1,1,5 },{ 0,0,5,-1,0,5,1,0,5,1,1,5 },{ 0,0,5,0,-1,5,0,1,5,1,-1,5 } },
	{ { 0,0,6,1,0,6,-1,0,6,1,-1,6 },{ 0,0,6,0,1,6,0,-1,6,-1,-1,6 },{ 0,0,6,1,0,6,-1,0,6,-1,1,6 },{ 0,0,6,0,-1,6,0,1,6,1,1,6 } },
	{ { 0,0,8,-1,0,8,1,0,8,0,1,8 },{ 0,0,8,0,-1,8,0,1,8,1,0,8 },{ 0,0,8,-1,0,8,1,0,8,0,-1,8 },{ 0,0,8,-1,0,8,0,-1,8,0,1,8 } },
};

enum { EMPTY, BRICK, WALL };
char arTile[3][4] = { "  ","■","□" };
int board[BW + 2][BH + 2];
int nx, ny;
int brick, rot;
int score = 0, max_score = 0;
time_t startTime;
int colors[7] = { 0,1,2,3,4,5,6 };
int colorsList[BW + 2][BH + 2] = { 0 };

int main()
{
	int nFrame, nStay;
	int x, y;

	showcursor(FALSE);
	randomize();
	clrscr();
	read_file();
	InitializeGameTime();
	system("title Tetris(team11)");


	// 가장자리는 벽, 나머지는 빈 공간으로 초기화한다.
	for (x = 0; x < BW + 2; x++) {
		for (y = 0; y < BH + 2; y++) {
			if (y == 0 || y == BH + 1 || x == 0 || x == BW + 1 || x == 11 || x > 11 && y == 10) {
				board[x][y] = WALL;
			}
			else {
				board[x][y] = EMPTY;
			}
		}
	}
	DrawScreen();
	nFrame = 20;
	// 전체 게임 루프
	for (; 1;) {
		brick = random(sizeof(Shape) / sizeof(Shape[0]));
		nx = 5;
		ny = 3;
		rot = 0;
		PrintBrick(TRUE);
		if (GetAround(nx, ny, brick, rot) != EMPTY) break;

		// 벽돌 하나가 바닥에 닿을 때까지의 루프
		nStay = nFrame;
		for (; 2;) {
			if (--nStay == 0) {
				nStay = nFrame;
				if (MoveDown()) break;
			}
			if (ProcessKey()) break;
			//n은 하락속도 조절

			int n = ControlSpeed(score);
			delay(1000 / n);
		}
	}
	clrscr();
	putsxy(30, 12, "G A M E  O V E R");
	gotoxy(30, 12);
	printf("게임 점수: %d점 최고 기록: %d", score, max_score);
	showcursor(TRUE);
}

void DrawScreen()
{
	color(7);
	for (int x = 0; x < BW + 2; x++) {
		for (int y = 0; y < BH + 2; y++) {
			color(7);
			gotoxy(BX + x * 2, BY + y);
			//putsxy(BX + x * 2, BY + y, arTile[board[x][y]]);
			printf(arTile[board[x][y]]);
		}
	}

	read_file();
	putsxy(30, 3, "Tetris Ver 1.0");
	gotoxy(30, 4);
	printf("현재 점수: %d", score);
	gotoxy(30, 5);
	printf("난이도: ");
	gotoxy(30, 6);
	printf("최고 기록: %d", max_score);
}

BOOL ProcessKey()
{
	if (kbhit()) {
		int ch = getch();
		if (ch == 0xE0 || ch == 0) {
			ch = getch();
			switch (ch) {
			case LEFT:
				if (GetAround(nx - 1, ny, brick, rot) == EMPTY) {
					PrintBrick(FALSE);
					nx--;
					PrintBrick(TRUE);
				}
				break;
			case RIGHT:
				if (GetAround(nx + 1, ny, brick, rot) == EMPTY) {
					PrintBrick(FALSE);
					nx++;
					PrintBrick(TRUE);
				}
				break;
			case UP:
				if (GetAround(nx, ny, brick, (rot + 1) % 4) == EMPTY) {
					PrintBrick(FALSE);
					rot = (rot + 1) % 4;
					PrintBrick(TRUE);
				}
				break;
			case DOWN:
				if (MoveDown()) {
					return TRUE;
				}
				break;
			}
		}
		else {
			switch (ch) {
			case ' ':
				while (MoveDown() == FALSE) { ; }
				return TRUE;
			case ESC:
				exit(0);
			}
		}
	}
	return FALSE;
}

void PrintBrick(BOOL Show) {
	playtime();
	for (int i = 0; i < 4; i++) {
		int blockColor = Shape[brick][rot][i].color; // 블록의 색상 정보 가져오기
		color(blockColor);
		gotoxy(BX + (Shape[brick][rot][i].x + nx) * 2, BY + Shape[brick][rot][i].y + ny);
		puts(arTile[Show ? BRICK : EMPTY]);
	}
	color(7); // 기본 텍스트 색상으로 되돌리기
}

int GetAround(int x, int y, int b, int r)
{
	int k = EMPTY;

	// 벽돌이 차지한 타일 모양 중 가장 큰 값을 찾는다.
	for (int i = 0; i < 4; i++) {
		k = max(k, board[x + Shape[b][r][i].x][y + Shape[b][r][i].y]);
	}
	return k;
}

BOOL MoveDown()
{
	// 바닥에 닿았으면 가득찼는지 점검하고 TRUE를 리턴한다.
	if (GetAround(nx, ny + 1, brick, rot) != EMPTY) {
		TestFull();
		return TRUE;
	}
	// 아직 공중에 떠 있으면 한칸 아래로 내린다.
	PrintBrick(FALSE);
	ny++;
	PrintBrick(TRUE);
	return FALSE;
}

void TestFull()
{

	// 바닥에 내려앉은 벽돌 기록과 동시에 색상 정보 저장
	for (int i = 0; i < 4; i++) {

		int x = nx + Shape[brick][rot][i].x;
		int y = ny + Shape[brick][rot][i].y;
		board[x][y] = BRICK;
		colorsList[x][y] = Shape[brick][rot][i].color;
	}

	// 수평으로 가득찬 벽돌 제거
	for (int y = 1; y < BH + 1; y++) {
		BOOL bFull = TRUE;
		for (int x = 1; x < 12; x++) {  //12 원래 BW
			if (board[x][y] == EMPTY) {
				bFull = FALSE;
				break;
			}
		}
		// 한줄이 가득 찼으면 이 줄을 제거한다.
		if (bFull) {
			for (int ty = y; ty > 1; ty--) {
				for (int x = 1; x < BW + 1; x++) {
					colorsList[x][ty] = colorsList[x][ty - 1];
					board[x][ty] = board[x][ty - 1];
				}
			}

			score += 100;
			if (score > max_score) {
				write_file();
			}

			DrawScreenWithColors(colorsList);
			delay(200);
		}
	}
}




void write_file() {
#pragma warning(disable:4996)
	FILE* fp;
	fp = fopen("max_score.txt", "w+");
	fwrite(&score, sizeof(int), 1, fp);
	fclose(fp);
}

void read_file()
{
#pragma warning(disable:4996)
	FILE* fp;
	fp = fopen("max_score.txt", "r + ");
	if (fp == NULL)
	{
		fp = fopen("max_score.txt", "w + ");
		fwrite(&max_score, sizeof(int), 1, fp);
	}
	fseek(fp, 0, 0);
	fread(&max_score, sizeof(int), 1, fp);
	fclose(fp);
}

void InitializeGameTime() {
	startTime = time(NULL);
}

// 현재 게임 경과 시간을 반환하는 함수
int GetGameTime() {
	time_t currentTime = time(NULL);
	return (int)(currentTime - startTime);
}

void color(int c)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void playtime() {
	gotoxy(30, 7);
	printf("게임 시간: %d초", GetGameTime());
}

void DrawScreenWithColors(int colorList[BW + 2][BH + 2])
{
	color(7);
	for (int x = 1; x < 11; x++) {
		for (int y = 1; y < BH + 1; y++) {
			gotoxy(BX + x * 2, BY + y);

			if (colorList[x][y] == 0) {  //!=
				//color(colorList[x][y]);
				board[x][y] = EMPTY;
				puts(arTile[EMPTY]);
			}
			else {
				board[x][y] = BRICK;
				color(colorList[x][y]);
				printf(arTile[board[x][y]]);
			}
		}
	}
	color(7);
	read_file();
	putsxy(30, 3, "Tetris Ver 1.0");
	gotoxy(30, 4);
	printf("현재 점수: %d", score);
	gotoxy(30, 5);
	printf("난이도: ");
	gotoxy(30, 6);
	printf("최고 기록: %d", max_score);
}

int ControlSpeed(int score) {
	int speed = 20;
	if (score > 200) {
		speed = 100;
	}
	return speed;
}