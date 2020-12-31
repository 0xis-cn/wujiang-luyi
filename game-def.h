#pragma once

static enum cell_colour {
	cell_wild = 100,		// 超出棋盘
	cell_bug = 101			// 未知错误
};

static enum modifier_return {
	modifier_ok,			// 继续进行
	modifier_abort			// 舍弃剩余处理操作, 全盘重算
};

typedef int exception_t;	// 暂时不知道用什么表示异常
const exception_t exception_null = 0;
const exception_t exception_wild = cell_wild; // TODO: rewrite exceptions.

struct disp_register {
	exception_t(*free_fall)(INT32, INT32, INT32, INT32);
	exception_t(*erase)(INT32, INT32);
};
