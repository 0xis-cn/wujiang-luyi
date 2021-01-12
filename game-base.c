#include "framework.h"
#include "forward.h"

static INT32 board[max_board_size][max_board_size], board_width, board_height;
static INT32 drop_above[max_board_size], cur_stamp, score = 0, qhead = 0;
static POINT erase_queue[max_queue];
struct rule_register rule;
#define score_slot -2

BOOL bs_start_game(const struct rule_register *cur_rule, mJson *json)
{
	rule = *cur_rule;
	cur_rule->on_new_game(board, json);
	dd_load_json(json);
	score = 0, qhead = 0;
	return TRUE;
}

BOOL on_user_trait(INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	INT32 cur;
	WCHAR buffer[MAX_LOADSTRING];
	if (xto != xfrom || yto != yfrom) set_swap(xto, yto, xfrom, yfrom);
	if ((cur = rule.first_trait(board, ++cur_stamp, xto, yto, xfrom, yfrom))) {
		do score += cur; while ((cur = rule.next_trait(board, ++cur_stamp)));
		swprintf_s(buffer, MAX_LOADSTRING, L"%d", score);
		dd_rewrite(score_slot, buffer);
	}
	else {
		if (xto != xfrom || yto != yfrom) set_swap(xto, yto, xfrom, yfrom);
	}

	if (cur_stamp > max_board_size) cur_stamp = 3;
	return TRUE;
}

BOOL create_map(INT32 height, INT32 width)
{
	if (board_width > max_board_size || board_height > max_board_size)
		return exception_wild;
	board_width = width, board_height = height;
	dd_new_map(height, width, board);
	return TRUE;
}

BOOL erase_colour(INT32 x, INT32 y)
{
	POINT xy = { .x = x,.y = y };
	if (drop_above[y] <= x) drop_above[y] = 1 + x;
	erase_queue[qhead++] = xy;
	dd_erase(x, y);
	return TRUE;
}

BOOL set_colour(INT32 x, INT32 y, INT32 colour)
{
	if (cell_wild != board[x][y]) {
		if (colour == cell_blank) return erase_colour(x, y);
		board[x][y] = colour;
		return TRUE;
	} else return exception_wild;
}

static INT32 isnotblank(INT32 x)
{
	return x != cell_wild && x != cell_blank;
}

BOOL erase_range(INT32 x, INT32 y, INT32 x1, INT32 y1)
{
	int i, j;
	for (i = x; i < x1; ++i) {
		for (j = y; j < y1; ++j) {
			POINT ij = { .x = i,.y = j };
			erase_queue[qhead++] = ij;
			dd_erase(i, j);
		}
	}
	return TRUE;
}

BOOL set_swap(INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	int temp = board[xfrom][yfrom];
	board[xfrom][yfrom] = board[xto][yto];
	board[xto][yto] = temp;
	dd_erase(xfrom, yfrom);
	dd_erase(xto, yto);
	dd_free_fall(xfrom, yfrom, xto, yto, board[xfrom][yfrom]);
	dd_free_fall(xto, yto, xfrom, yfrom, temp);
	return TRUE;
}

BOOL set_fall_cell(INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	int temp = board[xfrom][yfrom];
	board[xfrom][yfrom] = board[xto][yto];
	board[xto][yto] = temp;
	dd_erase(xfrom, yfrom);
	dd_free_fall(xto, yto, xfrom, yfrom, temp);
	return TRUE;
}

struct int32_maybe set_fall_range(INT32 x, INT32 y, INT32(*is_legal)(INT32))
{
	int i, j;
	struct int32_maybe result;

	result.exce = TRUE;

	for (i = x - 1, j = x; i >= 0; --i) {
		if (is_legal(board[i][y]) && i != --j)
			set_fall_cell(j, y, i, y);
	}

	result.data = j;
	return result;
}

BOOL set_random_fill(INT32 x, INT32 y)
{
	while (x--)
		dd_midway_display(x, y, board[x][y] = rule.midway_new_colour());
	return TRUE;
}

BOOL set_flush_with_fall(void)
{
	int i;
	while (qhead)
		qhead--, board[erase_queue[qhead].x][erase_queue[qhead].y] = cell_blank;
	for (i = 0; i < board_width; ++i)
		if (drop_above[i])
			set_random_fill(set_fall_range(drop_above[i], i, isnotblank).data, i);
	for (i = 0; i < board_width; ++i) drop_above[i] = 0;

	return TRUE;
}
