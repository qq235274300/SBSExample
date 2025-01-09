#pragma once
#include "Window.h"
#include "ChiliTimer.h"
class App
{
public:
	
	App();
	// master frame / message loop
	int Go();
	~App();
	
	void SetDllInstance(HINSTANCE dllInstance);
	HINSTANCE GetDllInstance();
public:
	 DWORD WINAPI CreateWindowInDll(LPVOID lpParam);
	 void TestApp();

private:
	void DoFrame();
private:
	 ChiliTimer timer;
	Window* wnd;
	HINSTANCE dll_Instance;
private:
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 80;
	
};
