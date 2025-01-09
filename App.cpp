#include "App.h"
#include "reshade.hpp"




App::App()
{
	reshade::log::message(reshade::log::level::error,  "App Class Construct!");
}

DWORD __stdcall App::CreateWindowInDll(LPVOID lpParam)
{
	wnd = new Window(800, 600, "Donkey Fart Box", dll_Instance);
	Go();
	return 0;
}

void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;

	wnd->Gfx().BeginFrame(c, c, 1.0f);
	wnd->Gfx().DrawTestTriangle(-timer.Peek(),0,2);
	wnd->Gfx().EndFrame();
}

App::~App()
{
	reshade::log::message(reshade::log::level::error, "App Class DeConstruct!");
}


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
