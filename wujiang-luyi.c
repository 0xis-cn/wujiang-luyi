/** wujiang-luyi.c - 生成程序窗口.
 * 本文件由 Visual Studio 自动生成. 本人用多行注释进行标注.
 * TODO: 修改菜单栏 (可能移除)
 *
 * 作者: 物灵
 * 日期: 2020-12-16
 */
#ifndef _DEBUG
#include "framework.h"
#include "wujiang-luyi.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

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

	// TODO: 在此处放置代码.

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

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{ /* 从消息队列取得消息到 msg */
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg); /* 翻译键盘消息 */
			DispatchMessage(&msg); /* 送往应用程序窗口函数 */
		}
	}

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
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT: /* 主程序执行 ShowWindows() 时 Windows 发出此消息 */
	{
		PAINTSTRUCT ps; /* 保存绘图信息的结构 */
		HDC hdc = BeginPaint(hWnd, &ps); /* 设备描述表代号 */
		RECT rect; /* 矩形结构 */
		GetClientRect(hWnd, &rect); /* 取得客户区的大小到 rect */
		DrawText(hdc, TEXT("I love ZYQ"), -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hWnd, &ps); /* 结束画图, 释放设备描述表代号 */
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0); /* 把 WM_QUIET 发至应用程序消息队列 */
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
#endif // _DEBUG
