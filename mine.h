#pragma once
#ifndef MINE_H_
#define MINE_H_

#define MF_W_MIN 10
#define MF_W_MAX 60
#define MF_H_MIN 10
#define MF_H_MAX 20

                                           // 화면 표시용 정보    | 게임 진행용 정보
#define MF_CLEAN           0x0000          // 0000 0000 0000 0000   0000 0000 0000 0000
#define MF_MINE            0x0001          // 0000 0000 0000 0000   0000 0000 0000 0001
#define MF_CURSOR          0x8000          // 0000 0000 0000 0000   1000 0000 0000 0000
#define MF_DISP_UNKNOWN    0x00010000      // 0000 0000 0000 0001   0000 0000 0000 0000
#define MF_DISP_EMPTY      0x00020000      // 0000 0000 0000 0010   0000 0000 0000 0000
#define MF_DISP_FLAG       0x00040000      // 0000 0000 0000 0100   0000 0000 0000 0000
#define MF_DISP_QUESTION   0x00080000      // 0000 0000 0000 1000   0000 0000 0000 0000
#define MF_DISP_MINE       0x00100000      // 0000 0000 0001 0000   0000 0000 0000 0000
#define MF_DISP_MINE_CLR   0x00E00000      // 0000 0000 1110 0000   0000 0000 0000 0000
#define MF_DISP_NUMBER     0x40000000      // 0100 XXXX 0000 0000   0000 0000 0000 0000 <-- XXXX 부분에는 주변의 지뢰 개수가 저장됨

#endif
