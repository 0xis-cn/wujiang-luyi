/** game-base.c - 游戏的工具集.
 * 计算线程及其消息循环在 game-thread.c 实现, only gamerule use functions here.
 * 实现视感的 displayer 负责队列的生成.
 *
 * 作者: 物灵
 * 日期: 2021-01-05
 */

#include "framework.h"
#include "game-def.h"

static INT32 board[max_board_size][max_board_size], board_width, board_height;
static INT32 drop_above[max_board_size], cur_stamp, score = 0;
struct disp_register displayer;
struct rule_register rule;

exce_t set_colour(INT32, INT32, INT32);
exce_t create_map(INT32, INT32);
exce_t set_fall_cell(INT32, INT32, INT32, INT32);
struct int32_maybe set_fall_range(INT32, INT32, INT32(*)(INT32));
exce_t set_flush_with_fall(VOID);
exce_t set_random_fill(INT32, INT32);
exce_t set_swap(INT32, INT32, INT32, INT32);
exce_t erase_colour(INT32, INT32);
exce_t erase_range(INT32, INT32, INT32, INT32);

exce_t start_game(
	const struct rule_register *cur_rule, const struct disp_register *cur_disp)
{
	displayer = *cur_disp;
	rule = *cur_rule;
	return exception_null;
}

exce_t base_write_self(struct base_register *base)
{
	base->size = sizeof(struct base_register);
	base->create_map = create_map;
	base->erase_colour = erase_colour;
	base->erase_range = erase_range;
	base->set_colour = set_colour;
	base->set_fall_cell = set_fall_cell;
	base->set_fall_range = set_fall_range;
	base->set_flush_with_fall = set_flush_with_fall;
	base->set_random_fill = set_random_fill;
	base->set_swap = set_swap;
	return exception_null;
}

// TODO: 分数的处理 (或许交与显示单元)
exce_t on_user_trait(INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	int cur;
	set_swap(xto, yto, xfrom, yfrom);
	if ((cur = rule.on_first_trait(board, ++cur_stamp, xto, yto, xfrom, yfrom)))
		do score += cur; while ((cur = rule.on_next_trait(board, ++cur_stamp)));
	else
		set_swap(xto, yto, xfrom, yfrom);

	if (cur_stamp > max_board_size) cur_stamp = 3;
	return exception_null;
}

exce_t create_map(INT32 height, INT32 width)
{
	if (board_width > max_board_size || board_height > max_board_size)
		return exception_wild;
	board_width = width, board_height = height;
	displayer.new_map(height, width, board);
	return exception_null;
}

exce_t erase_colour(INT32 x, INT32 y)
{
	if (drop_above[y] <= x) drop_above[y] = 1 + x;
	board[x][y] = cell_blank;
	displayer.erase(x, y);
	return exception_null;
}

exce_t set_colour(INT32 x, INT32 y, INT32 colour)
{
	if (cell_wild != board[x][y]) {
		if (colour == cell_blank) return erase_colour(x, y);
		board[x][y] = colour;
		return exception_null;
	} else return exception_wild;
}

static INT32 isnotblank(INT32 x)
{
	return x != cell_wild && x != cell_blank;
}

exce_t erase_range(INT32 x, INT32 y, INT32 x1, INT32 y1)
{
	int i, j;
	for (i = x; i < x1; ++i) {
		for (j = y; j < y1; ++j) {
			board[i][j] = cell_blank;
			displayer.erase(i, j);
		}
	}
	return exception_null;
}

/** set_swap() - 将 (xfrom, yfrom) 与 (xto, yto) 互换.
 * @xto: 到达点的横坐标.
 * @yto: 到达点的纵坐标.
 * @xfrom: 出发点的横坐标.
 * @yfrom: 出发点的纵坐标.
 *
 * 返回: 恒无异常.
 */
exce_t set_swap(INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	int temp = board[xfrom][yfrom];
	board[xfrom][yfrom] = board[xto][yto];
	board[xto][yto] = temp;
	displayer.erase(xfrom, yfrom);
	displayer.erase(xto, yto);
	displayer.free_fall(xfrom, yfrom, xto, yto, board[xfrom][yfrom]);
	displayer.free_fall(xto, yto, xfrom, yfrom, temp);
	return exception_null;
}

/** set_fall_cell() - 用 (xfrom, yfrom) 覆盖 (xto, yto).
 * @xto: 到达点的横坐标.
 * @yto: 到达点的纵坐标.
 * @xfrom: 出发点的横坐标.
 * @yfrom: 出发点的纵坐标.
 *
 * 返回: 恒无异常.
 * 
 * 注意: 未修改!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */
exce_t set_fall_cell(INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	int temp = board[xfrom][yfrom];
	board[xfrom][yfrom] = board[xto][yto];
	board[xto][yto] = temp;
	displayer.erase(xfrom, yfrom);
	displayer.free_fall(xto, yto, xfrom, yfrom, temp);
	return exception_null;
}

/** set_fall_range() - 对 ([0..x], y) 进行自由下落 (不含 x).
 * @x: 单元格的横坐标.
 * @y: 单元格的纵坐标.
 *
 * 返回: 整数 r 表示 [0..r] (不含 r) 为空, 恒无异常.
 * 
 * 历史版本:
 * 2020-12-20 创建
 * 2020-01-05 废除原有基于两次循环的复杂做法
 */
struct int32_maybe set_fall_range(INT32 x, INT32 y, INT32(*is_legal)(INT32))
{
	int i, j;
	struct int32_maybe result;

	result.exce = exception_null;

	for (i = x - 1, j = x; i >= 0; --i) {
		if (is_legal(board[i][y]) && i != --j)
			set_fall_cell(j, y, i, y);
	}

	result.data = j;
	return result;
}

exce_t set_random_fill(INT32 x, INT32 y)
{
	while (x--)
		displayer.midway_display(x, y, board[x][y] = rule.midway_new_colour());
	return exception_null;
}

exce_t set_flush_with_fall(void)
{
	int i;
	for (i = 0; i < board_width; ++i)
		if (drop_above[i])
			set_random_fill(set_fall_range(drop_above[i], i, isnotblank).data, i);
	displayer.flush();
	for (i = 0; i < board_width; ++i) drop_above[i] = 0;

	return exception_null;
}
