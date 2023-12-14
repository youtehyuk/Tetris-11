#define _CRT_NONSTDC_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>


#define randomize() srand((unsigned)time(NULL))
#define random(n) (rand() % (n))
#define delay(n) Sleep(n)
#define clrscr() system("cls")
#define gotoxy(x,y) { COORD Cur = {x, y}; \
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),Cur);}
#define showcursor(bShow) { CONSOLE_CURSOR_INFO CurInfo = {20, bShow}; \
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&CurInfo); }


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

void write_file();  //파일 없으면 쓰기
void read_file();   //파일에 저장된 스코어 읽기
int GetGameTime();
void color(int c); //색깔 변하는 코드, 1이면 1에 해당하는 색깔으로 변하기. 7은 흰색
void DrawScreenWithColors(int colorsToRemove[BW + 2][BH + 2]);
int ControlSpeed(int score); //속도 조절
void InitializeNextBrick();
void InitializeAfterBrick();


void PrintNextBrick(BOOL Show);
void PrintAfterBrick(BOOL Show);
void PrintInfo();
char checkGrade(int score);

//블록 자체에 색상을 추가
struct Point {
	int x, y;
	int color;
};
//블록 자체
struct Point Shape[][4][4] = {
	{ { 0,0,1,1,0,1,2,0,1,-1,0,1 },{ 0,0,1,0,1,1,0,-1,1,0,-2,1 },{ 0,0,1,1,0,1,2,0,1,-1,0,1 },{ 0,0,1,0,1,1,0,-1,1,0,-2 ,1} },
	{ { 0,0,2,1,0,2,0,1,2,1,1,2 },{ 0,0,2,1,0,2,0,1,2,1,1,2 },{ 0,0,2,1,0,2,0,1,2,1,1,2 },{ 0,0,2,1,0,2,0,1,2,1,1,2 } },
	{ { 0,0,3,-1,0,3,0,-1,3,1,-1,3 },{ 0,0,3,0,1,3,-1,0,3,-1,-1,3 },{ 0,0,3,-1,0,3,0,-1,3,1,-1,3 },{ 0,0,3,0,1,3,-1,0,3,-1,-1,3 } },
	{ { 0,0,4,-1,-1,4,0,-1,4,1,0,4 },{ 0,0,4,-1,0,4,-1,1,4,0,-1,4 },{ 0,0,4,-1,-1,4,0,-1,4,1,0,4 },{ 0,0,4,-1,0,4,-1,1,4,0,-1,4 } },
	{ { 0,0,5,-1,0,5,1,0,5,-1,-1,5 },{ 0,0,5,0,-1,5,0,1,5,-1,1,5 },{ 0,0,5,-1,0,5,1,0,5,1,1,5 },{ 0,0,5,0,-1,5,0,1,5,1,-1,5 } },
	{ { 0,0,6,1,0,6,-1,0,6,1,-1,6 },{ 0,0,6,0,1,6,0,-1,6,-1,-1,6 },{ 0,0,6,1,0,6,-1,0,6,-1,1,6 },{ 0,0,6,0,-1,6,0,1,6,1,1,6 } },
	{ { 0,0,12,-1,0,12,1,0,12,0,1,12 },{ 0,0,12,0,-1,12,0,1,12,1,0,12 },{ 0,0,12,-1,0,12,1,0,12,0,-1,12 },{ 0,0,12,-1,0,12,0,-1,12,0,1,12 } },
};

enum { EMPTY, BRICK, WALL };//블록의 상태
char arTile[3][4] = { "  ","■","□" }; //블록의 형태
int board[BW + 2][BH + 2]; //게임판의 영역
int nx, ny;
int brick, rot;
int score = 0, max_score = 0; //점수
time_t startTime;
int colorsList[BW + 2][BH + 2] = {};// 출력할 때 판의 있는 모든 블록의 색깔을 저장하기 위한 리스트
int nextBrick;
int afterBrick;
char grade;

int main()
{
	int nFrame, nStay;
	int x, y;
	showcursor(FALSE);
	randomize();
	clrscr();
	read_file();//프로그램 시작되면 최고기록 읽기
	startTime = time(NULL);

	// 가장자리는 벽, 나머지는 빈 공간으로 초기화한다. 칸의 상태가 벽인지 비어있는지 초기화하는 과정
	for (x = 0; x < BW + 2; x++) {
		for (y = 0; y < BH + 2; y++) {
			if (y == 0 || y == BH + 1 || x == 0 || x == BW + 1 || x == 11 || x > 11 && y == 8) {
				board[x][y] = WALL;
			}
			else {
				board[x][y] = EMPTY;
			}
		}
	}

	InitializeNextBrick();
	InitializeAfterBrick();
	DrawScreen();//초기화 상태에서 화면 출력
	nFrame = 20;
	// 전체 게임 루프
	for (; 1;) {
		if (score > max_score) {//최고점 찍을 때 파일을 써서 기록을 저장한다.
			write_file();
			read_file();
		}
		brick = nextBrick;
		nx = 5;
		ny = 3;
		rot = 0;

		PrintBrick(TRUE);//떨어지는 블록 출력하는 함수
		InitializeNextBrick();
		PrintNextBrick(TRUE);

		int save = afterBrick;
		nextBrick = save;
		PrintNextBrick(TRUE);
		InitializeAfterBrick();
		PrintAfterBrick(TRUE);


		if (GetAround(nx, ny, brick, rot) != EMPTY) break;

		// 벽돌 하나가 바닥에 닿을 때까지의 루프
		nStay = nFrame;
		for (; 2;) {
			if (--nStay == 0) {
				nStay = nFrame;
				if (MoveDown()) break;
			}
			if (ProcessKey()) break;
			//n은 하락속도 조절,클수록 하락속도 빠름
			int n = ControlSpeed(score);
			delay(1000 / n);
		}
	}
	delay(1000);
	clrscr();// 게임 실패하면 여기에 있는 코드 실행, 실패화면 만들 때 다루는 부분
	putsxy(30, 12, "G A M E  O V E R");
	gotoxy(30, 12);
	printf("게임 점수: %d점. 최고 기록: %d", score, max_score);
	showcursor(TRUE);
}

void DrawScreen()//게임판을 출력하는 함수, 일단 게임 초기화될때 한번 사용.
{
	color(7);
	for (int x = 0; x < BW + 2; x++) {
		for (int y = 0; y < BH + 2; y++) { //게임 영역안에서 작업
			color(7);
			gotoxy(BX + x * 2, BY + y);
			//putsxy(BX + x * 2, BY + y, arTile[board[x][y]]);
			printf(arTile[board[x][y]]);//좌표에 해당하는 모양을 출력한다.
		}
	}
	//오른쪽 위 부분에 해당되는 영역
	putsxy(30, 3, "Tetris Ver 1.0"); //putsxy는 커서를 해당 좌표로 옮기고 문구를 출력할 때 사용, %d,%f 같은 숫자를 출력 못함
	putsxy(29, 10, "Next Brick: ");
	putsxy(29, 16, "After Next Brick:");
}

BOOL ProcessKey()// 게임 컨트롤 부분,키보드 인식?,아마 다룰 필요없음
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

void PrintBrick(BOOL Show) {// 떨어지는 블록 출력하는 함수
	PrintInfo();
	for (int i = 0; i < 4; i++) {
		int blockColor = Shape[brick][rot][i].color; // 블록의 색상 정보 가져오기
		color(blockColor);
		gotoxy(BX + (Shape[brick][rot][i].x + nx) * 2, BY + Shape[brick][rot][i].y + ny);//떨어지는 좌표,여기에서 완쪽 중간에 해당
		puts(arTile[Show ? BRICK : EMPTY]);//블록 출력? 
	}
	color(7); // 기본 텍스트 색상으로 되돌리기
}


void PrintNextBrick(BOOL Show) {
	for (int x = 29; x < 45; x++) {
		for (int y = 11; y < 15; y++) {
			gotoxy(x, y);
			puts(arTile[EMPTY]);
		}
	}

	for (int i = 0; i < 4; i++) {
		int blockColor = Shape[nextBrick][rot][i].color; // 블록의 색상 정보 가져오기
		color(blockColor);
		gotoxy(36 + (Shape[nextBrick][rot][i].x * 2), 13 + Shape[nextBrick][rot][i].y);
		puts(arTile[Show ? BRICK : EMPTY]);
	}
	color(7); // 기본 텍스트 색상으로 되돌리기
}

void PrintAfterBrick(BOOL Show) {
	for (int x = 29; x < 45; x++) {
		for (int y = 18; y < 22; y++) {
			gotoxy(x, y);
			puts(arTile[EMPTY]);
		}
	}
	for (int i = 0; i < 4; i++) {
		int blockColor = Shape[afterBrick][rot][i].color; // 블록의 색상 정보 가져오기
		color(blockColor);
		gotoxy(36 + (Shape[afterBrick][rot][i].x * 2), 19 + Shape[afterBrick][rot][i].y);
		puts(arTile[Show ? BRICK : EMPTY]);
	}
	color(7); // 기본 텍스트 색상으로 되돌리기
}
int GetAround(int x, int y, int b, int r)//돌 수 있는지 확인하는 함수, 아마 다룰 필요없음
{
	int k = EMPTY;

	// 벽돌이 차지한 타일 모양 중 가장 큰 값을 찾는다.
	for (int i = 0; i < 4; i++) {
		k = max(k, board[x + Shape[b][r][i].x][y + Shape[b][r][i].y]);
	}
	return k;
}

BOOL MoveDown()// 바닥에 닿았는지 확인하는 함수
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

	score += 20;
	// 수평으로 가득찬 벽돌 제거
	for (int y = 1; y < BH + 1; y++) {
		BOOL bFull = TRUE;
		for (int x = 1; x < 12; x++) {  //12 원래 BW
			if (board[x][y] == EMPTY) {
				bFull = FALSE;
				break;
			}
		}
		// 한줄이 가득 찼으면 이 줄을 제거하고 모든 블록 정보를 한칸 씩 내린다. 색깔 정보도 같이 옮긴다.
		if (bFull) {
			for (int ty = y; ty > 1; ty--) {
				for (int x = 1; x < BW + 1; x++) {
					colorsList[x][ty] = colorsList[x][ty - 1];
					board[x][ty] = board[x][ty - 1];
				}
			}
			score += 100;//줄이 제거되면 점수 더한다.

			if (score > max_score) {//최고점 찍을 때 파일을 써서 기록을 저장한다.
				write_file();
				read_file();
			}
			DrawScreenWithColors(colorsList);//전에 모든 블록에 저장되어 있는 색깔 리스트를 전달하고 색깔 있는 블록으로 출력하기
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

// 현재 게임 경과 시간을 반환하는 함수
int GetGameTime() {
	time_t currentTime = time(NULL);
	return (int)(currentTime - startTime);
}

void color(int c)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}


void DrawScreenWithColors(int colorList[BW + 2][BH + 2])
{
	color(7);
	for (int x = 1; x < 11; x++) {
		for (int y = 1; y < BH + 1; y++) {
			gotoxy(BX + x * 2, BY + y);

			if (colorList[x][y] == 0) {//색이 저장되어 있지 않으면 빈칸으로 설정한다.
				board[x][y] = EMPTY;
				puts(arTile[EMPTY]);
			}
			else {
				board[x][y] = BRICK;//블록이면 그 색깔을 유지하고 줄이 제거된 상태에서 출력한다
				color(colorList[x][y]);
				printf(arTile[board[x][y]]);
			}
		}
	}
	color(7);
}

//score에 따른 속도 조절.
int ControlSpeed(int score) {
	int speed = 20;
	if (score > 6000) 	speed = 80;
	else if (score > 5000) speed = 70;
	else if (score > 4000) speed = 60;
	else if (score > 3000) speed = 50;
	else if (score > 2000) speed = 40;
	else if (score > 1000) speed = 30;
	return speed;
}


void InitializeNextBrick() {
	nextBrick = random(sizeof(Shape) / sizeof(Shape[0]));
}

void InitializeAfterBrick() {
	afterBrick = random(sizeof(Shape) / sizeof(Shape[0]));
}

void PrintInfo() {
	grade = checkGrade(score);
	gotoxy(30, 4);
	printf("난이도: %c", grade);
	gotoxy(30, 5);
	printf("현재 점수: %d", score);
	gotoxy(30, 6);
	printf("최고 기록: %d", max_score);
	gotoxy(30, 7);
	printf("게임 시간: %d초", GetGameTime());
}

char checkGrade(int score) {
	char grade;
	if (score > 6000) 	grade = 'S';
	else if (score > 5000) grade = 'A';
	else if (score > 4000) grade = 'B';
	else if (score > 3000) grade = 'C';
	else if (score > 2000) grade = 'D';
	else if (score > 1000) grade = 'E';
	else grade = 'F';
	return grade;
}
