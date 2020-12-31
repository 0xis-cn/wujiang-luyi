#pragma once

static enum cell_colour {
	cell_wild = 100,		// ��������
	cell_bug = 101			// δ֪����
};

static enum modifier_return {
	modifier_ok,			// ��������
	modifier_abort			// ����ʣ�ദ�����, ȫ������
};

typedef int exception_t;	// ��ʱ��֪����ʲô��ʾ�쳣
const exception_t exception_null = 0;
const exception_t exception_wild = cell_wild; // TODO: rewrite exceptions.

struct disp_register {
	exception_t(*free_fall)(INT32, INT32, INT32, INT32);
	exception_t(*erase)(INT32, INT32);
};
