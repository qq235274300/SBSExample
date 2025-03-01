#include "App.h"

#include "Box.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include "GDIPlusManager.h"
#include <iostream>

GDIPlusManager gdipm;

App::App()
{
	reshade::log::message(reshade::log::level::error,  "App Class Construct!");
}

DWORD __stdcall App::CreateWindowInDll(LPVOID lpParam)
{
	//wnd = new Window(2048, 768, "Donkey Fart Box", dll_Instance);

	class Factory
	{
	public:
		Factory(Graphics &gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{

			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
				);
			case 1:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist,
					odist, rdist
				);
			case 2:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist,
					odist, rdist
				);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
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
		std::uniform_int_distribution<int> typedist { 0,2};
	};

	
	/*drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, Factory { wnd->Gfx() });*/

	//D:\\Happy3D\\GRSD3D12Sample\\Debug\\x64
	//const auto s = Surface::FromFile("D:\\Happy3D\\GRSD3D12Sample\\Debug\\x64\\kappa50.png");

	//wnd->Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));


	//Go();

	return 0;
}

void App::TestApp()
{
	//wnd = new Window(2048,768, "Donkey Fart Box", dll_Instance);

	class Factory
	{
	public:
		Factory(Graphics &gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{

			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
				);
			case 1:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist,
					odist, rdist
				);
			case 2:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist,
					odist, rdist
				);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
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
	//const auto s = Surface::FromFile("Images\\kappa50.png");
	//Factory f(wnd->Gfx());
	//D:\\Happy3D\\GRSD3D12Sample\\Debug\\x64
	//const auto s = Surface::FromFile("D:\\Happy3D\\GRSD3D12Sample\\Debug\\x64\\kappa50.png");

// 	drawables.reserve(nDrawables);
// 	std::generate_n(std::back_inserter(drawables), nDrawables, Factory { wnd->Gfx() });
// 
// 	wnd->Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));

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

Window *App::GetWindow()
{
	return wnd;
}

void App::CopyRTVResource(ID3D11Device* pDevice,ID3D11Resource *From, reshade::api::resource_desc textureDesc)
{
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//srvDesc.Format = (DXGI_FORMAT )textureDesc.texture.format;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MostDetailedMip = 0;
	//srvDesc.Texture2D.MipLevels = 1;

	//if (From == nullptr || pDevice == nullptr) {
	//	// 处理错误，可能是资源没有正确创建或加载
	//	std::cerr << "Error: The resource is null!" << std::endl;
	//	return;
	//}


	//HRESULT hr = pDevice->CreateShaderResourceView(From, &srvDesc, &pTextureView);
	//if (FAILED(hr)) {
	//	// 处理错误
	//	std::cerr << "Failed to create shader resource view!" << std::endl;
	//}

	/*wnd->Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd->Gfx().EndFrame();*/

	//wnd->Gfx().pContext->OMSetRenderTargets(0, nullptr, nullptr);
	
	//wnd->Gfx().pContext->PSSetShaderResources(0u, 1u, pTextureView.GetAddressOf());
	/*Microsoft::WRL::ComPtr<ID3D11Resource> pSrvResource;
	wnd->Gfx().pTarget->GetResource(&pSrvResource);
	wnd->Gfx().pContext->CopyResource(From, pSrvResource.Get());*/
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
