#pragma once
typedef int exce_t;	// ��ʱ��֪����ʲô��ʾ�쳣

#define max_board_size 100
#define max_revise_size 65535


#include "framework.h"

struct int32_maybe {
	INT32 data;
	exce_t exce;
};

enum cell_colour {
	cell_wild = 100,		// ��������
	cell_bug = 101,			// δ֪����
	cell_blank = 102		// ʲôҲû��
};

enum modifier_return {
	modifier_ok,			// ��������
	modifier_abort			// ����ʣ�ദ�����, ȫ������
};

#define exception_null 0
#define exception_wild cell_wild // TODO: rewrite exceptions.

struct disp_register {
	size_t size;
	exce_t(*new_map)(INT32, INT32, INT32(*)[max_board_size]);
	exce_t(*midway_display)(INT32, INT32, INT32);
	exce_t(*erase)(INT32, INT32);
	exce_t(*free_fall)(INT32, INT32, INT32, INT32, INT32);
	exce_t(*flush)(VOID);
};

struct rule_register {
	size_t size;
	exce_t(*on_new_game)(INT32(*)[max_board_size]);
	INT32(*on_first_trait)(
		const INT32(*)[max_board_size], INT32, INT32, INT32, INT32, INT32);
	INT32(*on_next_trait)(const INT32(*)[max_board_size], INT32);
	INT32(*midway_new_colour)(VOID); // ��;���������Ϸ���ɫ
};

struct base_register {
	size_t size;
	exce_t(*set_colour)(INT32, INT32, INT32);
	exce_t(*create_map)(INT32, INT32);
	exce_t(*set_fall_cell)(INT32, INT32, INT32, INT32);
	struct int32_maybe(*set_fall_range)(INT32, INT32, INT32(*)(INT32));
	exce_t(*set_flush_with_fall)(VOID);
	exce_t(*set_random_fill)(INT32, INT32);
	exce_t(*set_swap)(INT32, INT32, INT32, INT32);
	exce_t(*erase_colour)(INT32, INT32);
	exce_t(*erase_range)(INT32, INT32, INT32, INT32);
};
