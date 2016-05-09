#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "getch.h"
#include "mine.h"

int numOfMines = 0;		// 지뢰 개수
int newGame = 1;		// 방금 시작된 게임인지 확인
int flagCount = 0;		// 깃발 개수
int clearValidate = 0;	// 클리어 판정용 카운터

void changeColor(char *color)
{
	printf(color);
}

// 게임 화면 업데이트
void updateDisplay(int **f, int *w, int *h, int *cx, int *cy)
{
	int i, j;
	int isCursor;
	int num;
	char *colors[8] = {_COLOR_ONE, _COLOR_TWO, _COLOR_THREE,
					 _COLOR_FOUR, _COLOR_FIVE, _COLOR_SIX,
					 _COLOR_NORMAL, _COLOR_NORMAL};
	char *colors_cur[8] = {_COLOR_ONE_CUR, _COLOR_TWO_CUR, _COLOR_THREE_CUR,
						 _COLOR_FOUR_CUR, _COLOR_FIVE_CUR, _COLOR_SIX_CUR,
						 _COLOR_NORMAL_CUR, _COLOR_NORMAL_CUR};

	system("clear");

	printf("===== 지뢰 찾기 === 지뢰 : %03d/%03d개 === (%02d, %02d) =====\n\n", flagCount, numOfMines, *cx, *cy);
	if (newGame)
	{
		changeColor("\e[92m");
		printf("\t아무 데나 밟으면 지뢰가 배치되고 게임이 시작됩니다!\n\n");
		changeColor(_COLOR_NORMAL);
	}
	for (i=0; i<*h; i++)
	{
		printf("\t");
		for (j=0; j<*w; j++)
		{
			isCursor = f[i][j] & _MF_CURSOR;
			if (isCursor) changeColor(_COLOR_NORMAL_CUR);
			if (f[i][j] & _MF_DISP_UNKNOWN) printf(".");
			if (f[i][j] & _MF_DISP_FLAG)
			{
				changeColor(isCursor ? _COLOR_FLAG_CUR : _COLOR_FLAG);
				printf("F");
				changeColor(_COLOR_NORMAL);
			}
			if (f[i][j] & _MF_DISP_QUESTION) printf("?");
			if (f[i][j] & _MF_DISP_EMPTY)
			{
				changeColor(isCursor ? _COLOR_GROUND_CUR : _COLOR_GROUND);
				printf(" ");
				changeColor(_COLOR_NORMAL);
			}
			if (f[i][j] & _MF_DISP_NUMBER)
			{
				num = (f[i][j] % 0x10000000) / 0x01000000;
				changeColor(isCursor ? colors_cur[num-1] : colors[num-1]);
				printf("%d", num);
				changeColor(_COLOR_NORMAL);
			}
			if (f[i][j] & _MF_DISP_MINE)
			{
				changeColor(_COLOR_EXPLOSION);
				printf("@");
				changeColor(_COLOR_NORMAL);
			}
			if (f[i][j] & _MF_DISP_MINE_CLR)
			{
				changeColor(_COLOR_CLEARED);
				printf("@");
				changeColor(_COLOR_NORMAL);
			}
			if (f[i][j] & _MF_CURSOR) changeColor(_COLOR_NORMAL);
		}
		printf("\n");
	}
	printf("\n");
	printf("화살표 키 = 이동\tf = 깃발\tq = 물음표\ts = 밟기\n");
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
			changeColor(_COLOR_FLAG);
			printf("게임 오버! 지뢰를 밟았습니다!\n");
			changeColor(_COLOR_NORMAL);
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
				changeColor("\e[92m");
				printf("축하합니다! 모든 지뢰를 찾았습니다!\n");
				changeColor(_COLOR_NORMAL);
				break;
			}
		}
		updateDisplay(minefield, &fW, &fH, &cX, &cY);
	}

	for (i=0; i<fH; i++) free(minefield[i]);
	free(minefield);

	return 0;
}
