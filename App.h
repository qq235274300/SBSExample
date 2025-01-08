#pragma once
#include "Window.h"

class App
{
public:
	
	App();
	// master frame / message loop
	int Go();
	~App();


	static App* GetInstance();
	static App* appInstance;
	

private:
	void DoFrame();
private:
	
	Window wnd;
	
	
};
