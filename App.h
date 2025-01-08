#pragma once
#include "Window.h"

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

private:
	void DoFrame();
private:
	
	Window* wnd;
	HINSTANCE dll_Instance;
	
};
