/** game-base.c - 游戏的工具集.
 * 计算线程及其消息循环在 game-thread.c 实现,
 * only gamerule use functions here.
 *
 * 作者: 物灵
 * 日期: 2021-01-05
 */

#include "framework.h"
#include "game-def.h"

#define max_board_size 100
#define max_revise_size 65535

static INT32 board[max_board_size][max_board_size], board_width, board_height;
struct disp_register displayer;

exce_t set_size(INT32 width, INT32 height)
{
	if (board_width > max_board_size || board_height > max_board_size)
		return exception_wild;
	board_width = width, board_height = height;
	return exception_null;
}

// This function is to determine whether to keep
exce_t set_colour(INT32 x, INT32 y, INT32 colour)
{
	if (cell_wild != get_colour(x, y)) {
		board[x][y] = colour;
		return exception_null;
	} else return exception_wild;
}

exce_t erase_colour(INT32 x, INT32 y)
{
	board[x][y] = cell_blank;
	displayer.erase(x, y);
	return exception_null;
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

/** set_fall_cell() - 将 (xfrom, yfrom) 与 (xto, yto) 互换.
 * @xto: 到达点的横坐标.
 * @yto: 到达点的纵坐标.
 * @xfrom: 出发点的横坐标.
 * @yfrom: 出发点的纵坐标.
 *
 * 返回: 恒无异常.
 */
exce_t set_swap(INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	int temp = get_colour(xfrom, yfrom);
	set_colour(xfrom, yfrom, get_colour(xto, yto));
	set_colour(xto, yto, temp);
	displayer.erase(xfrom, yfrom);
	displayer.erase(xto, yto);
	displayer.free_fall(xfrom, yfrom, xto, yto, get_colour(xfrom, yfrom));
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
 */
exce_t set_fall_cell(INT32 xto, INT32 yto, INT32 xfrom, INT32 yfrom)
{
	int temp = get_colour(xfrom, yfrom);
	set_colour(xfrom, yfrom, get_colour(xto, yto));
	set_colour(xto, yto, temp);
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

exce_t set_force_draw(void)
{
	int i, j;
	for (i = 0; i < board_width; ++i)
		for (j = 0; j < board_height; ++j)
			displayer.display(i, j, board[i][j]);

	return exception_null;
}

exce_t set_flush(void)
{
// TODO: step 1: call free fall
// TODO: step 2: add new points
}

