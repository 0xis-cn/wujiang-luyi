// �ƻ����ö�̬���ӿ�ʵ����Ϸ������Ȳ��, ��ʱ�䲻��.
// rule_register ��Ϊ��ʹ��̬���ӵ���Ϸ�������ʹ��.

#include "forward.h"

const static struct base_register base = {
	.size = sizeof(struct base_register),
	.create_map = create_map,
	.erase_colour = erase_colour,
	.erase_range = erase_range,
	.set_colour = set_colour,
	.set_fall_cell = set_fall_cell,
	.set_fall_range = set_fall_range,
	.set_flush_with_fall = set_flush_with_fall,
	.set_random_fill = set_random_fill,
	.set_swap = set_swap
};

static struct rule_register rule_list = {
	.size = sizeof(struct rule_register),
	.on_new_game = rd_on_new_game,
	.first_trait = rd_first_trait,
	.next_trait = rd_next_trait,
	.midway_new_colour = rd_midway_new_colour,
	.next = NULL
}, *preferred_rule = &rule_list;

BOOL loader_start_game(mJson *json)
{
	return bs_start_game(preferred_rule, json);
}
