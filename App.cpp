#include "App.h"

App *App::appInstance = nullptr;
App::App()
	:
	wnd(3840, 1080, "The Donkey Fart Box")
{
	
}

void App::DoFrame()
{
	//wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().EndFrame();
}

App::~App()
{}

App* App::GetInstance()
{
	if (appInstance == nullptr)
	{
		appInstance = new App();
		appInstance->Go();
	}
	return appInstance;
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
