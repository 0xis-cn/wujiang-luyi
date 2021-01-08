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
static INT32 drop_above[max_board_size], cur_stamp;
struct disp_register displayer;
struct rule_register rule;

// 此函数的参数不应为 VOID, 留待修改
// TODO: 游戏规则维护棋盘大小, 应从彼处传给此处
exce_t on_load(VOID)
{
	return exception_null;
}

// TODO: 分数的处理 (或许交与显示单元)
exce_t on_user_trait(INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	set_swap(xto, yto, xfrom, yfrom);
	if (rule.on_user_trait(board, ++cur_stamp))
		while (rule.on_user_trait(board, ++cur_stamp));
	else
		set_swap(xto, yto, xfrom, yfrom);

	if (cur_stamp > max_board_size) cur_stamp = 3;

	return exception_null;
}

/* 可能错误
exce_t acknowledge_rule(struct rule_register cur_rule)
{
	rule = cur_rule;
	return exception_null;
}*/

exce_t set_draw_map(INT32 width, INT32 height)
{
	if (board_width > max_board_size || board_height > max_board_size)
		return exception_wild;
	board_width = width, board_height = height;
	displayer.new_map(width, height, board);
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
	if (cell_wild != get_colour(x, y)) {
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
		if (is_legal(get_colour(i, y)) && i != --j)
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

/** set_move() - 似乎没有用, 考虑废弃.
exce_t set_move(INT32 xto, INT32 yto,
				INT32 xto1, INT32 yto1, INT32 xfrom, INT32 yfrom)
{
	int i, j;
	static int buffer[max_board_size][max_board_size];

	for (i = 0; xto + i < xto1; ++i)
		memcpy(buffer[i], yfrom + board[xfrom + i], sizeof(INT32) * (yto1 - yto));
	erase_range(xfrom, yfrom, xto1 - xto + xfrom, yto1 - yto + yfrom);

	for (i = 0; xto + i < xto1; ++i) {
		memcpy(yto + board[xto + i], buffer[i], sizeof(INT32) * (yto1 - yto));
		for (j = 0; yto + j < yto1; ++j)
			displayer.free_fall(xto + i, yto + j, xfrom + i, yfrom + j, buffer[i][j]);
	}

	return exception_null;
}
*/

exce_t set_flush_with_fall(void)
{
	int i;
	for (i = 0; i < board_width; ++i)
		if (drop_above[i])
			set_random_fill(set_fall_range(drop_above[i], i, isnotblank).data, i);
	displayer.flush();
	for (i = 0; i < board_width; ++i) drop_above[i] = 0;
}
