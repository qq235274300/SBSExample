#include "App.h"
#include "reshade.hpp"




App::App()
{
	reshade::log::message(reshade::log::level::error,  "App Class Construct!");
}

DWORD __stdcall App::CreateWindowInDll(LPVOID lpParam)
{
	wnd = new Window(800, 300, "Donkey Fart Box", dll_Instance);
	Go();
	return 0;
}

void App::DoFrame()
{
	//wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	//wnd->Gfx().EndFrame();
}

App::~App()
{}


void App::SetDllInstance(HINSTANCE dllInstance)
{
	dll_Instance = dllInstance;
}

HINSTANCE App::GetDllInstance()
{
	
	return dll_Instance;
}


int App::Go()
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}
