/** game-base.c - 游戏的工具集.
 * 计算线程及其消息循环在 game-thread.c 实现, 该文件调用本文件的函数.
 *
 * 作者: 物灵
 * 日期: 2020-12-20
 */

#include "framework.h"
#include "game-def.h"

#define max_board_size 100
#define max_revise_size 65535

static INT32 board[max_board_size][max_board_size], board_width, board_height;
struct disp_register displayer;

static struct revise_list {
	INT32 x, y, priority;
} revise_pool[max_revise_size];

static INT32 sort_as_priority(void *u, void *v)
{
	return ((struct revise_list *)v)->priority
		- ((struct revise_list *)u)->priority;
}

/** get_colour() - 查询单元格上的棋子种类.
 * @x: 单元格的横坐标.
 * @y: 单元格的纵坐标.
 *
 * Return:
 * * %cell_wild - 所查询单元格不在棋盘范围.
 * * 其他 - 单元格的实际棋子种类.
 */
INT32 get_colour(INT32 x, INT32 y)
{
	if (x < 0 || y < 0 || x >= board_width || y >= board_height)
		return (INT32)cell_wild;
	else return board[x][y];
}

exception_t set_colour(INT32 x, INT32 y, INT32 colour)
{
	if (cell_wild != get_colour(x, y)) {
		board[x][y] = colour;
		return exception_null;
	} else return exception_wild;
}

exception_t set_fall_down(INT32 x, INT32 y, INT32(*is_legal)(INT32))
{
	exception_t	e;

	while (cell_wild != get_colour(x, y)) {
		INT32 drag = x;
		INT32 temp;

		while (!is_legal(temp = get_colour(drag, y))) ++drag;
		if (x == drag) continue;
		if (exception_null != (e = set_colour(drag, y, get_colour(x, y))))
			goto catcher;
		if (exception_null != (e = set_colour(x, y, temp)))
			goto catcher;
		if (exception_null != (e = displayer.free_fall(x, y, drag, y)))
			goto catcher;
	}
	return exception_null;
catcher:
	return e; // TODO: rewrite exceptions.
}


