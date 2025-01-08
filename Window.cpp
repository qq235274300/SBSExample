#include "Window.h"
#include <sstream>
#include "resource.h"
#include "reshade.hpp"
#include "App.h"

// Window Class Stuff
//Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	///*:
	//hInst(GetInstance())*/
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	//wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	//wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	/*wc.hIcon = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON2),
		IMAGE_ICON, 32, 32, 0
		));*/
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	/*wc.hIconSm = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON2),
		IMAGE_ICON, 16, 16, 0
		));*/

	if (!RegisterClassEx(&wc))
	{
		MessageBoxA(NULL, "Error register Window class", "Error", MB_OK);
		
	}
}

Window::WindowClass::~WindowClass()
{
	//UnregisterClass(wndClassName, GetInstance());
}

const char *Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

//HINSTANCE Window::WindowClass::GetInstance() noexcept
//{
//	//return wndClass.hInst;
//}





// Window Stuff
Window::Window(int width, int height, const char *name, HINSTANCE _hInst)
	:width(width), height(height),hInst(_hInst)
{


	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	//wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = nullptr;
	/*wc.hIcon = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON2),
		IMAGE_ICON, 32, 32, 0
		));*/
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = "Chili Direct3D Engine Window";
	wc.hIconSm = nullptr;
	/*wc.hIconSm = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON2),
		IMAGE_ICON, 16, 16, 0
		));*/

	if (!RegisterClassEx(&wc))
	{
		MessageBoxA(NULL, "Error register Window class", "Error", MB_OK);

	};

	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;

	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{

	};
	// create window & get hWnd
	hWnd = CreateWindow(
		"Chili Direct3D Engine Window", name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, hInst, this
	);
	// show window
	if (hWnd == nullptr)
	{
		reshade::log::message(reshade::log::level::error, "Create Chili Window Failed");
		// Init ImGui Win32 Impl
	}
	reshade::log::message(reshade::log::level::error, "Create Chili Window Successed");
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	//pGfx = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	reshade::log::message(reshade::log::level::error, "Destroy Chili Window Successed");
	DestroyWindow(hWnd);
}


void Window::SetTitle(const std::string &title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		
	}

}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// check for quit because peekmessage does not signal this via return val
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return msg.wParam;
		}

		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

Graphics &Window::Gfx() const
{
	return *pGfx;
}




LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{

	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW *const pCreate = reinterpret_cast<CREATESTRUCTW *>(lParam);
		Window *const pWnd = static_cast<Window *>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve ptr to window class
	Window *const pWnd = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{



	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0); return 0;
		break;
		

	case WM_KILLFOCUS:
		break;
		/********************KEYBOARD MESSAGE******************************/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// stifle this keyboard message if imgui wants to capture
		break;


	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
};


