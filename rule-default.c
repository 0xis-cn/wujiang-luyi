/** rule-default.c - Default Gamerule.
 * Gamerules other than this is loaded by DLL.
 * 本程序会注册 Classic 和 Taobao 两个游戏规则. (undetermined)
 * TODO: 在主线程 srand()!
 *
 * 作者: 物灵
 * 日期: 2021-01-01
 */

#include <stdlib.h>
#include "game-def.h"

struct base_register base;
static INT32 board_width, board_height;

INT32 timestamp[max_board_size][max_board_size];

VOID load(struct base_register cur_base)
{
	base = cur_base;
	
}

INT32 check_stamp(INT32 x, INT32 y, INT32 cur_time)
{
	INT32 result = (cur_time != timestamp[x][y]);
	timestamp[x][y] = cur_time;
	return result;
}

INT32 on_user_trait(board_t board, INT32 cur_time)
{
	INT32 result = 0;
	INT32 i, j;

	for (i = 0; i < board_height; ++i)
		for (j = 0; j < board_width; ++j) {
			if (i + 2 < board_height &&
				board[i][j] == board[i + 1][j] &&
				board[i][j] == board[i + 2][j]
			)
				result += check_stamp(i, j, cur_time);
			if (j + 2 < board_width &&
				board[i][j] == board[i][j + 1] &&
				board[i][j] == board[i][j + 2]
			)
				result += check_stamp(i, j, cur_time);
		}
	if (result) base.setter_flush_with_fall();
	return result;
}

INT32 midway_new_colour(VOID)
{
	return 4000 + (rand() & 7);
}
