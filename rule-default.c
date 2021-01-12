#include <stdlib.h>
#include <time.h>
#include "forward.h"

static INT32 board_width, board_height, colours;
INT32 timestamp[max_board_size][max_board_size];

static inline INT32 check_stamp(INT32 x, INT32 y, INT32 cur_time)
{
	if (cur_time != timestamp[x][y]) {
		timestamp[x][y] = cur_time;
		erase_colour(x, y);
		return 1;
	} else return 0;
}

INT32 rd_next_trait(const INT32(*board)[max_board_size], INT32 cur_time)
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
	if (result) set_flush_with_fall();
	return result;
}

INT32 rd_first_trait(const INT32(*board)[max_board_size],
	INT32 cur_time, INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	if ((xto == xfrom && yto == yfrom + 1) ||
		(xto == xfrom && yto == yfrom - 1) ||
		(yto == yfrom && xto == xfrom + 1) ||
		(yto == yfrom && xto == xfrom - 1)) {
		int nextify = rd_next_trait(board, cur_time);
		return nextify;
	} else return 0;
}

INT32 rd_midway_new_colour(VOID)
{
	return 1024 ^ (rand() % colours);
}

BOOL rd_on_new_game(INT32(*board)[max_board_size], mJson *json)
{
	int i, j;
	board_height = mjson_get_int_or_default(json, "height", 12);
	board_width = mjson_get_int_or_default(json, "width", 12);
	colours = mjson_get_int_or_default(json, "colours", 8);

	for (i = 0; i < board_height; ++i)
		for (j = 0; j < board_width; ++j)
			board[i][j] = rd_midway_new_colour();

	create_map(board_height, board_width);
	for (j = max_board_size; rd_next_trait(board, j); ++j);
	dd_flush();
	return TRUE;
}

