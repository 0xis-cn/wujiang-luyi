#pragma once
typedef int exce_t;	// 暂时不知道用什么表示异常

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
	exce_t(*display)(INT32, INT32, INT32);
	exce_t(*erase)(INT32, INT32);
	exce_t(*free_fall)(INT32, INT32, INT32, INT32, INT32);
};

struct revise_list {
	INT32 x, y, priority;
};

/*struct rule_register {

};*/
