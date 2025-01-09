#include "App.h"
#include "reshade.hpp"
#include "Box.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"


App::App()
{
	reshade::log::message(reshade::log::level::error,  "App Class Construct!");
}

DWORD __stdcall App::CreateWindowInDll(LPVOID lpParam)
{
	wnd = new Window(800, 600, "Donkey Fart Box", dll_Instance);

	class Factory
	{
	public:
		Factory(Graphics &gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{

			return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
			);
			//switch (typedist(rng))
			//{
			//	/*case 0:
			//		return std::make_unique<Pyramid>(
			//			gfx, rng, adist, ddist,
			//			odist, rdist
			//		);*/
			//case 1:
			//	return std::make_unique<Box>(
			//		gfx, rng, adist, ddist,
			//		odist, rdist, bdist
			//	);
			//	/*	case 2:
			//			return std::make_unique<Melon>(
			//				gfx, rng, adist, ddist,
			//				odist, rdist, longdist, latdist
			//			);*/
			//default:
			//	assert(false && "bad drawable type in factory");
			//	return {};
			//}
		}
	private:
		Graphics &gfx;
		std::mt19937 rng { std::random_device{}() };
		std::uniform_real_distribution<float> adist { 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist { 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist { 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist { 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist { 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist { 5,20 };
		std::uniform_int_distribution<int> longdist { 10,40 };
		std::uniform_int_distribution<int> typedist { 0,2 };
	};

	Factory f(wnd->Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	wnd->Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));


	Go();

	return 0;
}

void App::TestApp()
{
	wnd = new Window(800, 600, "Donkey Fart Box", dll_Instance);

	class Factory
	{
	public:
		Factory(Graphics &gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{

			return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
			);
			//switch (typedist(rng))
			//{
			//	/*case 0:
			//		return std::make_unique<Pyramid>(
			//			gfx, rng, adist, ddist,
			//			odist, rdist
			//		);*/
			//case 1:
			//	return std::make_unique<Box>(
			//		gfx, rng, adist, ddist,
			//		odist, rdist, bdist
			//	);
			//	/*	case 2:
			//			return std::make_unique<Melon>(
			//				gfx, rng, adist, ddist,
			//				odist, rdist, longdist, latdist
			//			);*/
			//default:
			//	assert(false && "bad drawable type in factory");
			//	return {};
			//}
		}
	private:
		Graphics &gfx;
		std::mt19937 rng { std::random_device{}() };
		std::uniform_real_distribution<float> adist { 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist { 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist { 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist { 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist { 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist { 5,20 };
		std::uniform_int_distribution<int> longdist { 10,40 };
		std::uniform_int_distribution<int> typedist { 0,2 };
	};

	Factory f(wnd->Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	wnd->Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 2.0f));

	Go();
}

void App::DoFrame()
{
	const auto dt = timer.Mark();
	wnd->Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	for (auto &d : drawables)
	{
		d->Update(dt);
		d->Draw(wnd->Gfx());
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
