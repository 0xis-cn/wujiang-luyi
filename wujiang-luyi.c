#include "game-def.h"
#include "wujiang-luyi.h"

enum game_status { st_blank, st_opening, st_setting } cur_status = st_blank;

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
mJson *cur_json;

// 此代码模块中包含的函数的前向声明:

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//
//  函数: wWinMain()
//
//  目标: 程序的入口函数。
//
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{ /* 诸多例程作 WinMain(). 此处 wWinMain() 意指万国码环境. */
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	mJson *load_json(VOID);
	cur_json = load_json();

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WJLY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WJLY));

	MSG msg;
	GetMessage(&msg, NULL, 0, 0);
	INT64 time_now, time_pre = 0;

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else if (cur_status == st_opening) { // 仅当游戏进行中才调用
			extern BOOL on_beat(HDC); // 显示单元提供
			time_now = GetTickCount64();
			if (time_now - time_pre >= refresh_time) {
				time_pre = time_now;
				on_beat(GetDC(msg.hwnd));
			}
		}
	}

	BOOL mjson_write(mJson *json_obj);
	mjson_write(cur_json);

	return (int)msg.wParam; /* 带着 msg 的 wpara 参数返回操系 */
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
/* 定义窗口类变量, 并填写窗口类结构各条款, 形成初始化的窗口类 */
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW; /* 窗口的式样 */
	wcex.lpfnWndProc = WndProc; /* 窗口函数的地址 */
	wcex.cbClsExtra = 0; /* 额外空间 */
	wcex.cbWndExtra = 0; /* 额外空间 */
	wcex.hInstance = hInstance; /* 实例代号, 自 WinMain 传入本函数 */
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WJLY)); /* 载入图标 */
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); /* 载入鼠标 */
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); /* 背景画刷 */
 	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WJLY); /* 菜单名 */
	wcex.lpszClassName = szWindowClass; /* 窗口类名 */
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex); /* 对窗口类进行注册, 返回注册是否成功 */
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中, 我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(
		szWindowClass, /* 窗口类名 (全局) */
		szTitle, /* 窗口标题 */
		WS_OVERLAPPEDWINDOW, /* 窗口式样 */
		CW_USEDEFAULT, /* 窗口 x 坐标 */
		0, /* 窗口 y 坐标*/
		CW_USEDEFAULT, /* 窗口 x 方向的大小*/
		0, /* 窗口 y 方向的大小 */
		NULL, /* 父窗口代号 */
		NULL, /* 菜单代号*/
		hInstance, /* 实例代号 */
		NULL); /* 创建时的附加参数 */

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow); /* 显示 Windows 窗口 */
	UpdateWindow(hWnd); /* 修改 Windows 窗口 */

	return TRUE;
}

POINT point_by_lparam(LPARAM lpa)
{
	INT32 xpos = GET_X_LPARAM(lpa), ypos = GET_Y_LPARAM(lpa);
	extern INT32 which_horn_main(INT32), which_vert_main(INT32);
	POINT result = { .y = which_horn_main(xpos), .x = which_vert_main(ypos) };
	return result;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{ /* 窗口函数 */
	static BOOL mouse_drag = FALSE;
	static POINT from_point = { .x = -1, .y = -1 };
	static POINT to_point = { .x = -1, .y = -1 };
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		mouse_drag = TRUE;
		POINT now = point_by_lparam(lParam);
		extern BOOL draw_active(INT32, INT32, HDC);
		draw_active(now.x, now.y, GetDC(hWnd));
		if (now.x != -1 && now.y != -1) {
			if (from_point.x == -1 && from_point.y == -1) {
				from_point = now;
				to_point.x = to_point.y = -1;
			} else to_point = now;
		}
		break;
	}
	case WM_LBUTTONUP:
	case WM_MOUSELEAVE: // 离开窗口视同于松开
		mouse_drag = FALSE;
		if (to_point.x != -1 && to_point.y != -1) {
			extern BOOL draw_inactive(INT32, INT32, HDC);
			extern BOOL on_user_trait(INT32, INT32, INT32, INT32);
			draw_inactive(to_point.x, to_point.y, GetDC(hWnd));
			draw_inactive(from_point.x, from_point.y, GetDC(hWnd));
			on_user_trait(to_point.x, to_point.y, from_point.x, from_point.y);
			from_point.x = from_point.y = to_point.x = to_point.y = -1;
		}
		break;
	case WM_MOUSEMOVE:
		if (mouse_drag && to_point.x == -1) {
			POINT now = point_by_lparam(lParam);
			if ((now.x != -1 && now.y != -1) &&
				(now.x != from_point.x || now.y != from_point.y)) {
				to_point = now;
				draw_active(now.x, now.y, GetDC(hWnd));
			}
		}
		break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDC_STARTGAME:
			extern BOOL loader_start_game(mJson *); // 打开游戏
			cur_status = st_opening;
			loader_start_game(cur_json);
			break;
		case IDM_CONFIG:
			BOOL mjson_write(mJson * json_obj);
			mjson_write(cur_json);
			system("notepad " config_filename);
			mJson *load_json(VOID);
			cur_json = load_json();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT: /* 主程序执行 ShowWindows() 时 Windows 发出此消息 */
	{
		switch (cur_status) {
		case st_blank:
			break;
		case st_opening:
			extern BOOL on_redraw(HWND); // 重绘
			on_redraw(hWnd);
			break;
		case st_setting:
			break;
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0); /* 把 WM_QUIT 发至应用程序消息队列 */
		break; /* 返回 wWinMain() 中的消息循环 */
	default:
		return DefWindowProc(hWnd, message, wParam, lParam); /* 不尽处理 */
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
