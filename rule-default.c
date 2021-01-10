/** rule-default.c - Default Gamerule.
 * Gamerules other than this is loaded by DLL.
 * TODO: 在主线程 srand()!
 *
 * 作者: 物灵
 * 日期: 2021-01-01
 */

#include <stdlib.h>
#include "game-def.h"
#define zanshi 12

struct base_register base;
static INT32 board_width, board_height;
INT32 timestamp[max_board_size][max_board_size];

static inline INT32 check_stamp(INT32 x, INT32 y, INT32 cur_time)
{
	if (cur_time != timestamp[x][y]) {
		timestamp[x][y] = cur_time;
		base.erase_colour(x, y);
		return 1;
	} else return 0;
}

INT32 on_next_trait(const INT32(*board)[max_board_size], INT32 cur_time)
{
	INT32 result = 0;
	INT32 i, j;

	for (i = 0; i < board_height; ++i)
		for (j = 0; j < board_width; ++j) {
			if (i + 2 < board_height &&
				board[i][j] == board[i + 1][j] &&
				board[i][j] == board[i + 2][j]) {
				result += check_stamp(i, j, cur_time)
					+ check_stamp(i + 1, j, cur_time)
					+ check_stamp(i + 2, j, cur_time);
			}
			if (j + 2 < board_width &&
				board[i][j] == board[i][j + 1] &&
				board[i][j] == board[i][j + 2]) {
				result += check_stamp(i, j, cur_time)
					+ check_stamp(i, j + 1, cur_time)
					+ check_stamp(i, j + 2, cur_time);
			}
		}
	if (result) base.set_flush_with_fall();
	return result;
}

INT32 on_first_trait(const INT32(*board)[max_board_size],
	INT32 cur_time, INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	if ((xto == xfrom && yto == yfrom + 1) ||
		(xto == xfrom && yto == yfrom - 1) ||
		(yto == yfrom && xto == xfrom + 1) ||
		(yto == yfrom && xto == xfrom - 1)) {
		int nextify = on_next_trait(board, cur_time);
		return nextify;
	} else return 0;
}

INT32 midway_new_colour(VOID)
{
	return 1024 ^ (rand() & 7);
}

exce_t on_new_game(INT32(*board)[max_board_size])
{
	int i, j;

	for (i = 0; i < zanshi; ++i)
		for (j = 0; j < zanshi; ++j)
			board[i][j] = midway_new_colour();

	base.create_map(board_height = zanshi, board_width = zanshi);
	for (j = max_board_size; on_next_trait(board, j); ++j);
	return exception_null;
}

exce_t on_rule_load(
	const struct base_register *cur_base, struct rule_register *self)
{
	self->size = sizeof(struct rule_register);
	self->on_new_game = on_new_game;
	self->midway_new_colour = midway_new_colour;
	self->on_next_trait = on_next_trait;
	self->on_first_trait = on_first_trait;
	base = *cur_base;
	return exception_null;
}
