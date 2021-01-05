#pragma once
typedef int exce_t;	// ��ʱ��֪����ʲô��ʾ�쳣

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
