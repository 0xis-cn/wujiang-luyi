#pragma once
typedef int exce_t;	// 暂时不知道用什么表示异常

#define max_board_size 100
#define max_revise_size 65535

typedef const INT32(*board_t)[max_board_size];

#include "framework.h"

struct int32_maybe {
	INT32 data;
	exce_t exce;
};

enum cell_colour {
	cell_wild = 100,		// 超出棋盘
	cell_bug = 101,			// 未知错误
	cell_blank = 102		// 什么也没有
};

enum modifier_return {
	modifier_ok,			// 继续进行
	modifier_abort			// 舍弃剩余处理操作, 全盘重算
};

const exce_t exception_null = 0;
const exce_t exception_wild = cell_wild; // TODO: rewrite exceptions.

struct disp_register {
	size_t size;
	exce_t(*new_map)(INT32, INT32, board_t);
	exce_t(*midway_display)(INT32, INT32, INT32);
	exce_t(*erase)(INT32, INT32);
	exce_t(*free_fall)(INT32, INT32, INT32, INT32, INT32);
	exce_t(*flush)(void);
};

/*struct revise_list {
	INT32 x, y, priority;
};*/

struct rule_register {
	size_t size;
	INT32(*on_user_trait)(board_t, INT32);
	INT32(*midway_new_colour)(VOID); // 中途加入的随机合法颜色
};

struct base_register {
	size_t size;
	exce_t(*setter_colour)(INT32, INT32, INT32);
	exce_t(*setter_draw_map)(INT32, INT32);
	exce_t(*setter_fall_cell)(INT32, INT32, INT32, INT32);
	exce_t(*setter_fall_range)(INT32, INT32, INT32(*)(INT32));
	exce_t(*setter_flush_with_fall)(VOID);
	exce_t(*setter_random_fill)(INT32, INT32);
	exce_t(*setter_swap)(INT32, INT32, INT32, INT32);

	exce_t(*eraser_colour)(INT32, INT32);
	exce_t(*eraser_range)(INT32, INT32, INT32, INT32);
};
