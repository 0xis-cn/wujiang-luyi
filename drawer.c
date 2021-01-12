#include "forward.h"
#include "resource.h"

#define cell_top(x) (top + (INT32)((x) * side))
#define cell_left(x) (leftest + (INT32)((x) * side))
#define cell_right(x) cell_left(x + 1)
#define cell_bottom(x) cell_top(x + 1)
#define with_margin (INT32)(lucky_num * side) +
#define text_colour RGB(112, 112, 112)

static INT32 board[max_board_size][max_board_size];
static INT32 board_width, board_height, full_width, qhead = 0; // dd_new_map()
static INT32 horn, vert, side, top, leftest; // check_margin()
static POINT cat_queue[max_queue];
static WCHAR slot_text[3][MAX_LOADSTRING];
static HFONT dengxian = NULL;

static INT32 padding;
static double peel;

static HBRUSH brush_bright[max_colour], brush_dark[max_colour];

INT32 which_horn(INT32 x)
{
	INT32 result = (x - leftest) / side;
	return result < 0 ? 0 : result >= board_width ? board_width - 1 : result;
}

INT32 which_vert(INT32 x)
{
	INT32 result = (x - top) / side;
	return result < 0 ? 0 : result >= board_height ? board_height - 1 : result;
}

INT32 which_horn_main(INT32 x)
{
	INT32 result = (x - leftest) / side;
	return result < 0 ? -1 : result >= board_width ? -1 : result;
}

INT32 which_vert_main(INT32 x)
{
	INT32 result = (x - top) / side;
	return result < 0 ? -1 : result >= board_height ? -1 : result;
}

COLORREF hsv_colour(double h, INT32 s, INT32 v)
{
	INT32 darkest = v * 2.55;
	if (s) {
		INT32 brightest = darkest * (100 - s) / 100, opt = 6 * h;
		INT32 rem = (double)(6 * h - opt) * (INT32)(darkest - brightest);

		switch (opt) {
		case 0: return RGB(darkest, brightest + rem, brightest); break;
		case 1: return RGB(darkest - rem, darkest, brightest); break;
		case 2: return RGB(brightest, darkest, brightest + rem); break;
		case 3: return RGB(brightest, darkest - rem, darkest); break;
		case 4: return RGB(brightest + rem, brightest, darkest); break;
		case 5: return RGB(darkest, brightest, darkest - rem); break;
		default: return RGB(0, 0, 0); break;
		}
	} else return RGB(darkest, darkest, darkest);
}

HBRUSH brush_of_colour(INT32 x)
{
	static double h = lucky_num;
	if (x <= colour_reserved) return (HBRUSH)GRAY_BRUSH;
	if (!brush_bright[x]) {
		h += golden_ratio;
		if (h >= 1) h -= 1;
		brush_bright[x] = CreateSolidBrush(hsv_colour(h, 25, 80));
		brush_dark[x] = CreateSolidBrush(hsv_colour(h, 25, 50));
	}
	return brush_bright[x];
}

static BOOL check_margin(INT32 cur_horn, INT32 cur_vert)
{
	if (cur_horn != horn || cur_vert != vert) {
		horn = cur_horn, vert = cur_vert;

		if (horn * board_height > vert * full_width) { // 横向多余
			side = (vert - (padding << 1)) / board_height,
				top = padding, leftest = (horn - side * full_width) >> 1;
		} else { // 纵向多余
			side = (horn - (padding << 1)) / full_width,
				top = (vert - side * board_height) >> 1, leftest = padding;
		}

		dengxian = CreateFont(side << 1, 0, 0, 0, FW_LIGHT, 0, 0, 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, TEXT("等线")
		);
	}
	return side;
}

static inline BOOL draw_public(INT32 x, INT32 y, HDC hdc)
{
	SelectObject(hdc, GetStockObject(NULL_PEN));
	return Rectangle(hdc, with_margin cell_left(y),
		with_margin cell_top(x), cell_right(y), cell_bottom(x)); // 所需矩形
}

BOOL draw_inactive(INT32 x, INT32 y, HDC hdc)
{
	if (!hdc) return FALSE;
	SelectObject(hdc, brush_of_colour(board[x][y]));
	return draw_public(x, y, hdc);
}

BOOL draw_active(INT32 x, INT32 y, HDC hdc)
{
	if (!hdc) return FALSE;
	SelectObject(hdc, brush_dark[board[x][y]]);
	return draw_public(x, y, hdc);
}

BOOL on_redraw(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	RECT rect;

	GetClientRect(hWnd, &rect);
	if (!check_margin(rect.right - rect.left, rect.bottom - rect.top)) {
		EndPaint(hWnd, &ps);
		return FALSE;
	}

	for (INT32 i = which_vert(ps.rcPaint.top),
		iend = which_vert(ps.rcPaint.bottom); i <= iend; ++i)
		for (INT32 j = which_horn(ps.rcPaint.left),
			jend = which_horn(ps.rcPaint.right); j <= jend; ++j)
			draw_inactive(i, j, hdc);

	return EndPaint(hWnd, &ps);
}

BOOL dd_midway_display(INT32 x, INT32 y, INT32 colour)
{
	POINT xy = { .x = x, .y = y };
	board[x][y] = colour;
	cat_queue[qhead++] = xy;
	return TRUE;
}

BOOL dd_erase(INT32 x, INT32 y)
{
	return dd_midway_display(x, y, cell_blank);
}

BOOL dd_free_fall(INT32 a, INT32 b, INT32 c, INT32 d, INT32 e)
{
	return dd_midway_display(a, b, e);
}

BOOL dd_new_map(INT32 height, INT32 width,
	const INT32(*base_board)[max_board_size])
{
	board_height = height, board_width = width,
		full_width = (1 + side_bar) * board_width,
		memcpy(board, base_board, sizeof(board));
	dd_flush();
	return TRUE;
}

inline static BOOL draw_slot(INT32 pos, HDC hdc)
{
	if (dengxian) SelectObject(hdc, dengxian);
	SetTextColor(hdc, text_colour);
	TextOut(hdc, cell_left(board_width + 1),
		cell_top(((4 + pos) * 3)) + (side >> 1), slot_text[4 + pos],
		wcslen(slot_text[4 + pos]));
	return TRUE;
}

BOOL on_beat(HDC hdc)
{
	while (qhead) {
		--qhead;
		switch (cat_queue[qhead].x) {
		case -1:
			for (INT32 i = 0; i < board_height; ++i)
				for (INT32 j = 0; j < board_width; ++j)
					draw_inactive(i, j, hdc);
			break;
		case -2: case -3: case -4: draw_slot(cat_queue[qhead].x, hdc); break;
		default:
			draw_inactive(cat_queue[qhead].x, cat_queue[qhead].y, hdc);
		}
	}
	return TRUE;
}

BOOL dd_rewrite(INT32 pos, const WCHAR *value)
{
	if (qhead == max_queue || (pos != -2 && pos != -3 && pos != -4))
		return FALSE;
	POINT sv = { .x = pos, .y = 0 };
	cat_queue[qhead++] = sv;
	return wcscpy_s(slot_text[4 + pos], MAX_LOADSTRING, value);
}

BOOL dd_load_json(mJson *json)
{
	padding = mjson_get_int_or_default(json, "padding", 20);
	peel = mjson_get_real_or_default(json, "peel", 0.1);
	return TRUE;
}

BOOL dd_flush(VOID)
{
	const POINT all_screen = { .x = -1, .y = -1 };
	cat_queue[0] = all_screen;
	qhead = 1;
	return TRUE;
}
