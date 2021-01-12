#include "game-def.h"

INT32 rd_midway_new_colour(VOID);
INT32 rd_first_trait(const INT32(*)[max_board_size],
	INT32, INT32, INT32, INT32, INT32);
BOOL rd_on_new_game(INT32(*)[max_board_size], mJson *);
INT32 rd_next_trait(const INT32(*)[max_board_size], INT32);

BOOL bs_start_game(const struct rule_register *, mJson *);
BOOL set_colour(INT32, INT32, INT32);
BOOL create_map(INT32, INT32);
BOOL set_fall_cell(INT32, INT32, INT32, INT32);
struct int32_maybe set_fall_range(INT32, INT32, INT32(*)(INT32));
BOOL set_flush_with_fall(VOID);
BOOL set_random_fill(INT32, INT32);
BOOL set_swap(INT32, INT32, INT32, INT32);
BOOL erase_colour(INT32, INT32);
BOOL erase_range(INT32, INT32, INT32, INT32);

INT32 which_horn(INT32 x);
INT32 which_vert(INT32 x);
COLORREF hsv_colour(double h, INT32 s, INT32 v);
HBRUSH brush_of_colour(INT32 x);
BOOL on_redraw(HWND hWnd);
BOOL dd_midway_display(INT32 x, INT32 y, INT32 colour);
BOOL dd_erase(INT32 x, INT32 y);
BOOL dd_free_fall(INT32 a, INT32 b, INT32 c, INT32 d, INT32 e);
BOOL dd_new_map(INT32 height, INT32 width,
	const INT32(*base_board)[max_board_size]);
BOOL on_beat(HDC hdc);
BOOL dd_rewrite(INT32 pos, const WCHAR *value);
BOOL dd_load_json(mJson *json);
BOOL dd_flush(VOID);
