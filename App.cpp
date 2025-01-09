#include "App.h"
#include "reshade.hpp"
#include "Box.h"
#include <memory>



App::App()
{
	reshade::log::message(reshade::log::level::error,  "App Class Construct!");
}

DWORD __stdcall App::CreateWindowInDll(LPVOID lpParam)
{
	wnd = new Window(800, 600, "Donkey Fart Box", dll_Instance);

	std::mt19937 rng(std::random_device {}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	for (auto i = 0; i < 80; i++)
	{
		boxes.push_back(std::make_unique<Box>(
			wnd->Gfx(), rng, adist,
			ddist, odist, rdist
		));
	}
	wnd->Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));

	Go();
	return 0;
}

void App::DoFrame()
{
	/*const float c = sin(timer.Peek()) / 2.0f + 0.5f;

	wnd->Gfx().BeginFrame(c, c, 1.0f);
	wnd->Gfx().DrawTestTriangle(-timer.Peek(),0,2);
	wnd->Gfx().EndFrame();*/

	auto dt = timer.Mark();
	wnd->Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	for (auto &b : boxes)
	{
		b->Update(dt);
		b->Draw(wnd->Gfx());
	}
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
