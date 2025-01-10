
#pragma once
#include "ChiliWin.h"
#include <optional>
#include <memory>
#include <string>
#include "Graphics.h"
class Window
{

private:
	// singleton manages registration/cleanup of window class
	class WindowClass
	{
	public:
		static const char *GetName() noexcept;
		//static HINSTANCE GetInstance() noexcept;
		
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass &) = delete;
		WindowClass &operator=(const WindowClass &) = delete;
		static constexpr const char *wndClassName = "Chili Direct3D Engine Window";
		//static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char *name, HINSTANCE _hInst,HWND g_hWnd);
	~Window();
	Window(const Window &) = delete;
	//Window &operator=(const Window &) = delete;

	void SetTitle(const std::string &title);
	static std::optional<int> ProcessMessages()noexcept;

	Graphics &Gfx()const;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

private:
	int width;
	int height;
	//HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	HINSTANCE hInst;
	
};

