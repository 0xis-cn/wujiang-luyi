#pragma once

#define MAX_LOADSTRING 100
#define max_board_size 100
#define max_colour 4096
#define colour_reserved 1023
#define max_queue max_board_size * max_board_size
#define golden_ratio 0.618033988749895 // 黄金分割.
#define lucky_num 0.1
#define side_bar 0.5
#define refresh_time 100
#define config_filename "config.json"

#include "framework.h"

typedef struct mJson {
	struct mJson *next, *prev;
	enum mJsonType type;
	char *key;
	union mJsonData {
		struct dualNum { int integer; double real; } num;
		char *str;
		struct mJson *child;
	} data;
} mJson;

struct int32_maybe {
	INT32 data;
	BOOL exce;
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

#define exception_wild cell_wild // TODO: rewrite exceptions.

/*struct disp_register {
	size_t size;
	BOOL(*new_map)(INT32, INT32, const INT32(*)[max_board_size]);
	BOOL(*midway_display)(INT32, INT32, INT32);
	BOOL(*erase)(INT32, INT32);
	BOOL(*free_fall)(INT32, INT32, INT32, INT32, INT32);
	BOOL(*flush)(VOID);
	struct disp_register *next;
};*/

// 已注册的游戏规则.
struct rule_register {
	size_t size;
	BOOL(*on_new_game)(INT32(*)[max_board_size], mJson *);
	INT32(*first_trait)(
		const INT32(*)[max_board_size], INT32, INT32, INT32, INT32, INT32);
	INT32(*next_trait)(const INT32(*)[max_board_size], INT32);
	INT32(*midway_new_colour)(VOID); // 中途加入的随机合法颜色.
	struct rule_register *next;
};

struct base_register {
	size_t size;
	BOOL(*set_colour)(INT32, INT32, INT32);
	BOOL(*create_map)(INT32, INT32);
	BOOL(*set_fall_cell)(INT32, INT32, INT32, INT32);
	struct int32_maybe(*set_fall_range)(INT32, INT32, INT32(*)(INT32));
	BOOL(*set_flush_with_fall)(VOID);
	BOOL(*set_random_fill)(INT32, INT32);
	BOOL(*set_swap)(INT32, INT32, INT32, INT32);
	BOOL(*erase_colour)(INT32, INT32);
	BOOL(*erase_range)(INT32, INT32, INT32, INT32);
};


INT32 mjson_get_int_or_default(mJson *json_obj, const char *key, INT32 value);
double mjson_get_real_or_default(mJson *json_obj, const char *key, double value);
