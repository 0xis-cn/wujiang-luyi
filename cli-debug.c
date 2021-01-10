#ifdef _DEBUG

#include <stdio.h>
#include <time.h>
#include "game-def.h"

static INT32 board[max_board_size][max_board_size], bwqwq, bhqwq;

extern exce_t base_write_self(struct base_register *);
extern exce_t on_rule_load(
	const struct base_register *, struct rule_register *);
extern exce_t start_game(
	const struct rule_register *, const struct disp_register *);
extern exce_t on_user_trait(INT32, INT32, INT32, INT32);

exce_t threeinlove(int c, int d, int e)
{
	board[c][d] = e;
	return 0;
}


exce_t fiveinlove(int a, int b, int c, int d, int e)
{
	threeinlove(a, b, e);
	return 0;
}

exce_t flush(VOID)
{
	int i, j;
	printf("#%-4d", 0);
	for (i = 0; i < bhqwq; ++i)
		printf("%4d", i);
	for (i = 0; i < bwqwq; ++i) {
		printf("\n#%-4d", i);
		for (j = 0; j < bhqwq; ++j)
			printf("%4d", 1024 ^ board[i][j]);
	}
	puts("\nI love ZYQ.\n");
	return 0;
}

exce_t new_map(int a, int b, INT32(*haha)[max_board_size])
{
	bhqwq = a;
	bwqwq = b;
	int i, j;
	for (i = 0; i < a; ++i)
		for (j = 0; j < b; ++j)
			board[i][j] = haha[i][j];
	return flush();
}

exce_t verylazy(int a, int b)
{
	return 0;
}

int main(VOID)
{
	puts("I love ZYQ!\nThis is DEBUG of wujiang-luyi.");
	srand((unsigned)time(NULL));

	struct rule_register rule;
	struct disp_register disp;
	struct base_register base;

	disp.size = sizeof(struct disp_register);
	disp.erase = verylazy;
	disp.flush = flush;
	disp.new_map = new_map;
	disp.free_fall = fiveinlove;
	disp.midway_display = threeinlove;

	base_write_self(&base);
	on_rule_load(&base, &rule);
//  on_disp_load();
	start_game(&rule, &disp);

// 在这里接收输入的!! 不是 base!! 别忘了!!
	int a, b, c, d;
	while (4 == scanf("%d%d%d%d", &a, &b, &c, &d))
		on_user_trait(a, b, c, d);

	return 0;
}

#endif // _DEBUG
