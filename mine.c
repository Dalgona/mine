#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include "mine.h"

int rows, cols;

int numOfMines = 0;		// 지뢰 개수
int newGame = 1;		// 방금 시작된 게임인지 확인
int flagCount = 0;		// 깃발 개수
int clearValidate = 0;	// 클리어 판정용 카운터

void colorOn();

// 게임 화면 업데이트
void updateDisplay(int **f, int *w, int *h, int *cx, int *cy)
{
	int i, j;
	int isCursor;
	int num;
  int color;
  char ch;

  clear();
	mvprintw(0, 0, "===== Minesweeper === %03d mines out of %03d === (%02d, %02d) =====", flagCount, numOfMines, *cx, *cy);
	if (newGame)
	{
    attron(COLOR_PAIR(4));
		mvprintw(2, 8, "Step anywhere to start the game!");
    attroff(COLOR_PAIR(4));
	}

	for (i=0; i<*h; i++)
	{
		move(4 + i, 8);
		for (j=0; j<*w; j++)
		{
			if (f[i][j] & _MF_DISP_UNKNOWN)
      {
        color = 200;
        ch = '.';
      }
			else if (f[i][j] & _MF_DISP_FLAG)
			{
        color = 201;
				ch = 'F';
			}
			else if (f[i][j] & _MF_DISP_QUESTION)
      {
        color = 202;
        ch = '?';
      }
			else if (f[i][j] & _MF_DISP_EMPTY)
			{
        color = 203;
				ch = ' ';
			}
			else if (f[i][j] & _MF_DISP_NUMBER)
			{
				num = (f[i][j] % 0x10000000) / 0x01000000;
        color = 100 + num;
				ch = '0' + num;
			}
			else if (f[i][j] & _MF_DISP_MINE)
			{
        color = 204;
				ch = '@';
			}
			else if (f[i][j] & _MF_DISP_MINE_CLR)
			{
        color = 205;
				ch = '@';
			}

      attron(COLOR_PAIR(color));
      addch(ch);
      attroff(COLOR_PAIR(color));
		}
	}
	mvprintw(rows - 1, 0, "ARROW: MOVE    f: FLAG    q: NOTSURE    s: STEP");
  move(4 + *cy, 8 + *cx);

  refresh();
}

// 깃발 꽂기
void setFlag(int **f, int *cx, int *cy)
{
	int *cp = &f[*cy][*cx];

	if (*cp & _MF_DISP_UNKNOWN)
	{
		*cp -= _MF_DISP_UNKNOWN;
		*cp |= _MF_DISP_FLAG;
		flagCount++;
	}
	else if (*cp & _MF_DISP_QUESTION)
	{
		*cp -= _MF_DISP_QUESTION;
		*cp |= _MF_DISP_FLAG;
		flagCount++;
	}
	else if (*cp & _MF_DISP_FLAG)
	{
		*cp -= _MF_DISP_FLAG;
		*cp |= _MF_DISP_UNKNOWN;
		flagCount--;
	}
}

// 물음표
void setQuestionMark(int **f, int *cx, int *cy)
{
	int *cp = &f[*cy][*cx];

	if (*cp & _MF_DISP_UNKNOWN)
	{
		*cp -= _MF_DISP_UNKNOWN;
		*cp |= _MF_DISP_QUESTION;
	}
	else if (*cp & _MF_DISP_FLAG)
	{
		*cp -= _MF_DISP_FLAG;
		*cp |= _MF_DISP_QUESTION;
		flagCount--;
	}
	else if (*cp & _MF_DISP_QUESTION)
	{
		*cp -= _MF_DISP_QUESTION;
		*cp |= _MF_DISP_UNKNOWN;
	}
}

// 밟기
int step(int **f, int *w, int *h, int *cx, int *cy)
{
	int count = 0;
	int i, j;

	// 게임 시작 직후에는 어디를 밟더라도 지뢰가 터지지 않아야 함
	if (newGame)
	{
		newGame = 0;
		// 지뢰 뿌리기
		for (i=0; i<rand()%(*h)+rand()%6+1; i++)
		{
			for (j=0; j<rand()%(*w)+rand()%6+1; j++)
			{
				f[rand()%(*h)][rand()%(*w)] |= _MF_MINE;
			}
		}
		if (f[*cy][*cx] & _MF_MINE) f[*cy][*cx] -= _MF_MINE;
		// 지뢰 개수 세기
		for (i=0; i<*h; i++)
		{
			for (j=0; j<*w; j++) if (f[i][j] & _MF_MINE) numOfMines++;
		}
	}

	if (f[*cy][*cx] & _MF_DISP_UNKNOWN)
	{
		// 지뢰를 밟았을 경우
		if (f[*cy][*cx] & _MF_MINE)
		{
			return 0xFF;
		}
		// 지뢰를 밟지 않았을 경우
		else
		{
			for (i=*cy-1; i<=*cy+1; i++)
			{
				for (j=*cx-1; j<=*cx+1; j++)
				{
					if (i<0 || i>=*h || j<0 || j>=*w) continue;
					if (f[i][j] & _MF_MINE) count++;
				}
			}
			if (count==0)
			{
				f[*cy][*cx] -= _MF_DISP_UNKNOWN;
				f[*cy][*cx] |= _MF_DISP_EMPTY;
				for (i=*cy-1; i<=*cy+1; i++)
				{
					for (j=*cx-1; j<=*cx+1; j++)
					{
						if (i<0 || i>=*h || j<0 || j>=*w) continue;
						step(f, w, h, &j, &i);
					}
				}
			}
			else
			{
				f[*cy][*cx] -= _MF_DISP_UNKNOWN;
				f[*cy][*cx] |= _MF_DISP_NUMBER;
				f[*cy][*cx] |= (count << 24);
			}
			return 0;
		}
	}
	else return -1;
}

int main(void)
{
	int **minefield;
	int i, j, fW, fH, cX = 0, cY = 0;
	int key, stepResult = 0;

	// 의사 난수 시드 생성
	srand(time(NULL));

	// 지뢰밭 크기 입력
	while(1)
	{
		printf("지뢰밭의 가로 길이 (%d ~ %d)> ", _MF_W_MIN, _MF_W_MAX);
		scanf("%d", &fW);
		printf("지뢰밭의 세로 길이 (%d ~ %d)> ", _MF_H_MIN, _MF_H_MAX);
		scanf("%d", &fH);

		if(fW<_MF_W_MIN || fW>_MF_W_MAX || fH<_MF_H_MIN || fH>_MF_H_MAX) printf("[!] 입력값이 범위를 초과했습니다. 다시 입력해 주십시오.\n");
		else break;
	}

  initscr();
  getmaxyx(stdscr, rows, cols);
  cbreak();
  noecho();

  colorOn();

	// 지뢰밭 메모리에 할당 & 초기화
	printf("creating minefield...\n");
	minefield = (int **) malloc(sizeof(int *) * fH);
	for (i=0; i<fH; i++)
	{
		minefield[i] = (int *) malloc(sizeof(int) * fW);
		for (j=0; j<fW; j++) minefield[i][j] = _MF_CLEAN | _MF_DISP_UNKNOWN;
	}

	// 맨 왼쪽 위를 현위치로 설정
	minefield[cY][cX] |= _MF_CURSOR;

	updateDisplay(minefield, &fW, &fH, &cX, &cY);

	//메인 게임 루프
	while(1)
	{
		key = getch();
		switch (key)
		{
		case 65:	// 위쪽 화살표
			minefield[cY][cX] -= _MF_CURSOR;
			if (cY==0) cY = fH - 1; else cY--;
			minefield[cY][cX] |= _MF_CURSOR;
			break;
		case 68:	// 왼쪽 화살표
			minefield[cY][cX] -= _MF_CURSOR;
			if (cX==0) cX = fW - 1; else cX--;
			minefield[cY][cX] |= _MF_CURSOR;
			break;
		case 67:	// 오른쪽 화살표
			minefield[cY][cX] -= _MF_CURSOR;
			if (cX==fW - 1) cX = 0; else cX++;
			minefield[cY][cX] |= _MF_CURSOR;
			break;
		case 66:	// 아래쪽 화살표
			minefield[cY][cX] -= _MF_CURSOR;
			if (cY==fH - 1) cY = 0; else cY++;
			minefield[cY][cX] |= _MF_CURSOR;
			break;

		case 102:	// 'f'
			setFlag(minefield, &cX, &cY);
			break;
		case 113:	// 'q'
			setQuestionMark(minefield, &cX, &cY);
			break;
		case 115:	// 's'
			stepResult = step(minefield, &fW, &fH, &cX, &cY);
			break;
		}
		// 게임 오버 동작
		if (stepResult==0xFF)
		{
			for (i=0; i<fH; i++)
			{
				for (j=0; j<fW; j++) if (minefield[i][j] & _MF_MINE) minefield[i][j] = _MF_DISP_MINE;
			}
			updateDisplay(minefield, &fW, &fH, &cX, &cY);
      attron(COLOR_PAIR(2));
			mvprintw(rows - 2, 0, "GAME OVER! You stepped on a mine!\n");
      clrtoeol();
			mvprintw(rows - 1, 0, "Press any key to exit...");
      attroff(COLOR_PAIR(2));
      getch();
			break;
		}
		// 클리어 동작
		if (!newGame && numOfMines==flagCount)
		{
			clearValidate = 0;
			for (i=0; i<fH; i++)
			{
				for (j=0; j<fW; j++)
				{
					if (minefield[i][j] & _MF_MINE && minefield[i][j] & _MF_DISP_FLAG) clearValidate++;
				}
			}
			if (clearValidate==numOfMines)
			{
				for (i=0; i<fH; i++)
				{
					for (j=0; j<fW; j++) if (minefield[i][j] & _MF_MINE) minefield[i][j] = _MF_DISP_MINE_CLR;
				}
				updateDisplay(minefield, &fW, &fH, &cX, &cY);
        attron(COLOR_PAIR(3));
				mvprintw(rows - 2, 0, "CONGRATULATIONS! You've found all mine!\n");
        clrtoeol();
				mvprintw(rows - 1, 0, "Press any key to continue...");
        attroff(COLOR_PAIR(3));
        getch();
				break;
			}
		}
		updateDisplay(minefield, &fW, &fH, &cX, &cY);
	}

	for (i=0; i<fH; i++) free(minefield[i]);
	free(minefield);

  endwin();

	return 0;
}

void colorOn()
{
  if (has_colors())
  {
    start_color();
    init_pair(1, 255, 0);      // Guide text
    init_pair(2, 197, 0);      // Game over message
    init_pair(3, 118, 0);      // Game cleared message
    init_pair(4, 86, 0);       // New game message
    init_pair(101, 21, 250);   // Number 1
    init_pair(102, 22, 250);   // Number 2
    init_pair(103, 88, 250);   // Number 3
    init_pair(104, 18, 250);   // Number 4
    init_pair(105, 100, 250);  // Number 5
    init_pair(106, 24, 250);   // Number 6
    init_pair(107, 240, 250);  // Number 7
    init_pair(108, 240, 250);  // Number 8
    init_pair(109, 240, 250);  // Number 9
    init_pair(200, 250, 253);  // Unexplored
    init_pair(201, 196, 253);  // Flag
    init_pair(202, 235, 253);  // Question
    init_pair(203, 250, 250);  // Blank
    init_pair(204, 226, 202);  // Explosion
    init_pair(205, 235, 28);   // Correct
  }
}
