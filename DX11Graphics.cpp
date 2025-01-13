#include "DX11Graphics.h"
#include <DirectXMath.h>

#include <dxgi1_2.h>
#include <iostream>
#include "reshade.hpp"
#include <sstream>
//#include <dxgi.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;
using namespace DirectX;

#pragma comment  (lib,"d3d11.lib")

#pragma comment (lib,"D3DCompiler.lib")

#define MAX_BACKBUF_COUNT   3


#define ReleaseObject(object) if((object) != NULL) { object->Release(); object = NULL; }


struct ST_VERTEX_QUAD
{
	XMFLOAT4 m_v4Position;		//Position
	XMFLOAT4 m_vClr;		//Color
	XMFLOAT2 m_vTxc;		//Texcoord
};


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    switch (msg)
    {
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        bool isDown = (msg == WM_KEYDOWN);
        if (wparam == VK_ESCAPE)
            DestroyWindow(hwnd);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    case WM_SIZE:
    {
        break;
    }
    default:
        result = DefWindowProcW(hwnd, msg, wparam, lparam);
    }
    return result;
}


// Window Exception Stuff
DX11Graphics::Exception::Exception(int line, const char *file, HRESULT hr) noexcept
    :
    ChiliException(line, file),
    hr(hr)
{}
const char *DX11Graphics::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] " << GetErrorCode() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}
const char *DX11Graphics::Exception::GetType() const noexcept
{
    return "Chili Window Exception";
}
std::string DX11Graphics::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
    char *pMsgBuf = nullptr;
    DWORD nMsgLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
    );
    if (nMsgLen == 0)
    {
        return "Unidentified error code";
    }
    std::string errorString = pMsgBuf;
    LocalFree(pMsgBuf);
    return errorString;
}
HRESULT DX11Graphics::Exception::GetErrorCode() const noexcept
{
    return hr;
}
std::string DX11Graphics::Exception::GetErrorString() const noexcept
{
    return TranslateErrorCode(hr);
}

bool DX11Graphics::CreateDeviceAndSwapChain(HWND hwnd)
{
	Microsoft::WRL::ComPtr<IDXGIFactory1> pFactory;
	HRESULT hr;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
	if (FAILED(hr)) {
		throw std::runtime_error("IDXGIFactoryクラスの作成に失敗しました。");
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.BufferCount = MAX_BACKBUF_COUNT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //DXGI_SWAP_EFFECT_DISCARD DXGI_SWAP_EFFECT_SEQUENTIAL

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//DXGI_USAGE_BACK_BUFFER DXGI_USAGE_RENDER_TARGET_OUTPUT
	swapChainDesc.Flags = 0;
	//swapChainDesc.Flags = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	//フルスクリーンとウィンドモードの切り替えがしたい場合は、まずウィンドウモードとして生成することを推奨しているみたい
	swapChainDesc.Windowed = true;
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;


	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pContext
	);


	/*hr = pFactory->CreateSwapChain(d3d11Device, &swapChainDesc, &pSwapChain);
	if (FAILED(hr)) {
		throw std::runtime_error("IDXGISwapChainの作成に失敗");
	}*/

	if (pSwapChain) {
		reshade::log::message(reshade::log::level::info, ("host_resource_index: 1"));
	}
	else {
		reshade::log::message(reshade::log::level::info, ("host_resource_index: 2"));
	}

	// gain access to texture subresource in swap chain(back buffer)
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);

	// create depth stensil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	pDevice->CreateDepthStencilState(&dsDesc, &pDSState);
	// bind depth state
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);
	// create depth stensil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = WIDTH;
	descDepth.Height = HEIGHT;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
	// create view of depth stensil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	pDevice->CreateDepthStencilView(
		pDepthStencil.Get(), &descDSV, &pDSV
	);
	// bind depth stensil view to OM
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 2048.0f;
	vp.Height = 768.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);
	
	//全屏三角形



	ST_VERTEX_QUAD stTriangleVertices[] =
	{

		{ { -1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f },	{ 0.0f, 0.0f }  },
		{ { 1.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f },	{ 1.0f, 0.0f }  },
		{ { -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },	{ 0.0f, 1.0f }  },
		{ { 1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f },	{ 1.0f, 1.0f }  }


	};

	


	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(stTriangleVertices);
	bd.StructureByteStride = sizeof(ST_VERTEX_QUAD);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = stTriangleVertices;
	pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer);
	// Bind vertex buffer to pipeline
	const UINT stride = sizeof(ST_VERTEX_QUAD);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	// Create Sampler State
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pDevice->CreateSamplerState(&samplerDesc, &pSampler);
	//sample 是否现在绑定？
	pContext->PSSetSamplers(0, 1, pSampler.GetAddressOf());




	// create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;

	hr = D3DCompileFromFile(L"SBSExpand.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &pBlob, nullptr);
	hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	hr = D3DCompileFromFile(L"SBSExpand.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &pBlob, nullptr);
	// 创建顶点着色器
	hr = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// input (vertex) layout (2d position only)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	pDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	);

	// bind vertex layout
	pContext->IASetInputLayout(pInputLayout.Get());

	//bind render target 
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);
	 //Set primitive topology to triangle list (groups of 3 vertices)
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// configure viewport
	

	//pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u);


	return true;
}

void DX11Graphics::CreateSRV_forGameRTV(DXGI_FORMAT currentBackBufferFormat,  ID3D11Resource *currentBackBuffer)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = currentBackBufferFormat;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	pDevice->CreateShaderResourceView(currentBackBuffer, &srvDesc, &pSRV);

}

void DX11Graphics::TestCreateSwapChain(HINSTANCE hInstance)
{
    HWND hwnd;
    {
        WNDCLASSEXW winClass = {};
        winClass.cbSize = sizeof(WNDCLASSEXW);
        winClass.style = CS_HREDRAW | CS_VREDRAW;
        winClass.lpfnWndProc = &WndProc;
        winClass.hInstance = hInstance;
        winClass.hIcon = nullptr;
        winClass.lpszClassName = L"MyWindowClass";
        winClass.hIconSm = nullptr;

        if (!RegisterClassExW(&winClass)) {
            MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK);

        }

        RECT initialRect = { 0, 0, 1024, 768 };
        AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
        LONG initialWidth = initialRect.right - initialRect.left;
        LONG initialHeight = initialRect.bottom - initialRect.top;

        hwnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
                                winClass.lpszClassName,
                                L"01. Initialising Direct3D 11",
                                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                initialWidth,
                                initialHeight,
                                0, 0, hInstance, 0);

        if (!hwnd) {
            MessageBoxA(0, "CreateWindowEx failed", "Fatal Error", MB_OK);

        }
    }

    // Create D3D11 Device and Context
    ID3D11Device1 *d3d11Device;
    ID3D11DeviceContext1 *d3d11DeviceContext;
    {
        ID3D11Device *baseDevice;
        ID3D11DeviceContext *baseDeviceContext;
        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG_BUILD)
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT hResult = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE,
                                            0, creationFlags,
                                            featureLevels, ARRAYSIZE(featureLevels),
                                            D3D11_SDK_VERSION, &baseDevice,
                                            0, &baseDeviceContext);
        if (FAILED(hResult)) {
            MessageBoxA(0, "D3D11CreateDevice() failed", "Fatal Error", MB_OK);

        }

        // Get 1.1 interface of D3D11 Device and Context
        hResult = baseDevice->QueryInterface(__uuidof(ID3D11Device1), (void **)&d3d11Device);
        assert(SUCCEEDED(hResult));
        baseDevice->Release();

        hResult = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void **)&d3d11DeviceContext);
        assert(SUCCEEDED(hResult));
        baseDeviceContext->Release();
    }

#ifdef DEBUG_BUILD
    // Set up debug layer to break on D3D11 errors
    ID3D11Debug *d3dDebug = nullptr;
    d3d11Device->QueryInterface(__uuidof(ID3D11Debug), (void **)&d3dDebug);
    if (d3dDebug)
    {
        ID3D11InfoQueue *d3dInfoQueue = nullptr;
        if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&d3dInfoQueue)))
        {
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
            d3dInfoQueue->Release();
        }
        d3dDebug->Release();
    }
#endif

    // Create Swap Chain
    IDXGISwapChain1 *d3d11SwapChain;
    {
        // Get DXGI Factory (needed to create Swap Chain)
        IDXGIFactory2 *dxgiFactory;
        {
            IDXGIDevice1 *dxgiDevice;
            HRESULT hResult = d3d11Device->QueryInterface(__uuidof(IDXGIDevice1), (void **)&dxgiDevice);
            assert(SUCCEEDED(hResult));

            IDXGIAdapter *dxgiAdapter;
            hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
            assert(SUCCEEDED(hResult));
            dxgiDevice->Release();

            DXGI_ADAPTER_DESC adapterDesc;
            dxgiAdapter->GetDesc(&adapterDesc);

            OutputDebugStringA("Graphics Device: ");
            OutputDebugStringW(adapterDesc.Description);

            hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&dxgiFactory);
            assert(SUCCEEDED(hResult));
            dxgiAdapter->Release();
        }

        DXGI_SWAP_CHAIN_DESC1 d3d11SwapChainDesc = {};
        d3d11SwapChainDesc.Width = 0; // use window width
        d3d11SwapChainDesc.Height = 0; // use window height
        d3d11SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        d3d11SwapChainDesc.SampleDesc.Count = 1;
        d3d11SwapChainDesc.SampleDesc.Quality = 0;
        d3d11SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        d3d11SwapChainDesc.BufferCount = 2;
        d3d11SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        d3d11SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        d3d11SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        d3d11SwapChainDesc.Flags = 0;

        HRESULT hResult = dxgiFactory->CreateSwapChainForHwnd(d3d11Device, hwnd, &d3d11SwapChainDesc, 0, 0, &d3d11SwapChain);
        assert(SUCCEEDED(hResult));

        dxgiFactory->Release();
    }

    // Create Framebuffer Render Target
    ID3D11RenderTargetView *d3d11FrameBufferView;
    {
        ID3D11Texture2D *d3d11FrameBuffer;
        HRESULT hResult = d3d11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&d3d11FrameBuffer);
        assert(SUCCEEDED(hResult));

        hResult = d3d11Device->CreateRenderTargetView(d3d11FrameBuffer, 0, &d3d11FrameBufferView);
        assert(SUCCEEDED(hResult));
        d3d11FrameBuffer->Release();
    }

    // Main Loop
    bool isRunning = true;
    while (isRunning)
    {
        MSG msg = {};
        while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                isRunning = false;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        FLOAT backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
        d3d11DeviceContext->ClearRenderTargetView(d3d11FrameBufferView, backgroundColor);

        d3d11SwapChain->Present(1, 0);
    }


}

void DX11Graphics::Init_Resource(ID3D11Device *pDevice, HWND _hwnd, HINSTANCE hInstance)
{
    using namespace Microsoft::WRL;
    // namespace wrl = Microsoft::WRL;
    HRESULT hr;



    //IDXGIDevice2 *pDXGIDevice;
    //hr = pDevice->QueryInterface(__uuidof(IDXGIDevice2), (void **)&pDXGIDevice);

    //IDXGIAdapter *pDXGIAdapter;
    //hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

    //IDXGIFactory2 *pIDXGIFactory;
    //pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&pIDXGIFactory);

    //DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

    //swapChainDesc.Width = 800;
    //swapChainDesc.Height = 600;
    //swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_B8G8R8A8_UNORM
    //swapChainDesc.Stereo = false;
    //swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
    //swapChainDesc.SampleDesc.Quality = 0;
    //swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    //swapChainDesc.BufferCount = 2;
    //swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    //swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FOREGROUND_LAYER;
    //swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;//DXGI_ALPHA_MODE_PREMULTIPLIED 
    //swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;//DXGI_SCALING_ASPECT_RATIO_STRETCH  DXGI_SCALING_NONE

    //ComPtr<IDXGISwapChain1> swapChain;
    //hr = pIDXGIFactory->CreateSwapChainForCoreWindow(
    //   pDevice,
    //   reinterpret_cast<IUnknown *>(_hwnd),
    //   &swapChainDesc,
    //   nullptr,
    //   &swapChain
    //);






    try
    {

        HRESULT CheckMultisampleQualityLevels(
        DXGI_FORMAT Format,
        UINT SampleCount,
        UINT * pNumQualityLevels);


        // pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, mMultiSamplingCount, &mMultiSamplingQualityLevels);
        UINT createDeviceFlags = 0;
#if defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        //ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
        swapChainDesc.Width = 0;
        swapChainDesc.Height = 0;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R8G8B8A8_UNORM  DXGI_FORMAT_B8G8R8A8_UNORM  DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //DXGI_USAGE_RENDER_TARGET_OUTPUT DXGI_USAGE_BACK_BUFFER 
        swapChainDesc.BufferCount = 2;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//DXGI_SWAP_EFFECT_FLIP_DISCARD  DXGI_SWAP_EFFECT_DISCARD DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
        swapChainDesc.Flags = 0;
        swapChainDesc.Scaling = DXGI_SCALING_NONE; // DXGI_SCALING_STRETCH  DXGI_SCALING_NONE
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;//DXGI_ALPHA_MODE_IGNORE DXGI_ALPHA_MODE_UNSPECIFIED
        swapChainDesc.Stereo = false;

        ID3D11Device1 *d3d11Device;
        IDXGIDevice1 *dxgiDevice = nullptr;
        IDXGIAdapter *dxgiAdapter = nullptr;
        IDXGISwapChain1 *d3d11SwapChain = nullptr;




        hr = pDevice->QueryInterface(__uuidof(ID3D11Device1), (void **)&d3d11Device);
        assert(SUCCEEDED(hr));

#ifdef DEBUG_BUILD
        // Set up debug layer to break on D3D11 errors
        ID3D11Debug *d3dDebug = nullptr;
        d3d11Device->QueryInterface(__uuidof(ID3D11Debug), (void **)&d3dDebug);
        if (d3dDebug)
        {
            ID3D11InfoQueue *d3dInfoQueue = nullptr;
            if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&d3dInfoQueue)))
            {
                d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
                d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
                d3dInfoQueue->Release();
            }
            d3dDebug->Release();
        }
#endif

        HRESULT hr = d3d11Device->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void **>(&dxgiDevice));
        if (FAILED(hr))
        {
            // QueryInterface 失败时，抛出异常或者进行错误处理
            throw std::runtime_error("Failed to retrieve IDXGIDevice from ID3D11Device.");
        }

        if (FAILED(hr = dxgiDevice->GetAdapter(&dxgiAdapter)))
        {
            // ReleaseObject(dxgiDevice);
            throw std::runtime_error("IDXGIDevice::GetParent() failed retrieving adapter.");
        }

        IDXGIFactory2 *dxgiFactory = nullptr;
        if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void **>(&dxgiFactory))))
        {
            //ReleaseObject(dxgiDevice);
           // ReleaseObject(dxgiAdapter);
            throw std::runtime_error("IDXGIAdapter::GetParent() failed retrieving factory.");
        }


        if (dxgiFactory && pDevice && d3d11Device)
        {
            hr = dxgiFactory->CreateSwapChainForHwnd(reinterpret_cast<IUnknown *>(pDevice), _hwnd, &swapChainDesc, 0, 0, &d3d11SwapChain);
            //dxgiFactory->CreateSwapChainForCoreWindow(d3d11Device, reinterpret_cast<IUnknown *>(_hwnd), &swapChainDesc, 0, &d3d11SwapChain);
            // assert(SUCCEEDED(hr));
             // ReleaseObject(dxgiFactory);
        }
        if (d3d11SwapChain)
        {
            reshade::log::message(reshade::log::level::info, ("pSwapChain1: Live"));
        }
        else
        {
            reshade::log::message(reshade::log::level::info, ("pSwapChain1: Not Live"));
        }

        /*ID3D11Texture2D *d3d11FrameBuffer;
        ID3D11RenderTargetView *d3d11FrameBufferView;
        ID3D11DepthStencilView *depthBufferView;
        hr = pSwapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&d3d11FrameBuffer);
        assert(SUCCEEDED(hr));

        hr = pDevice->CreateRenderTargetView(d3d11FrameBuffer, 0, &d3d11FrameBufferView);
        assert(SUCCEEDED(hr));*/

    }
    catch (const ChiliException &e)
    {
        MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception &e)
    {
        MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

    //wrl::ComPtr<ID3D11Resource> pBackBuffer;
    //d3d11SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
    //pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);

    //struct Vertex
    //{
    //  struct
    //  {
    //      float x;
    //      float y;
    //      float z;
    //  } pos;
    //  struct
    //  {
    //      float r;
    //      float g;
    //      float b;
    //      float a;
    //  } color; // 颜色数据

    //  struct
    //  {
    //      float u;
    //      float v;
    //  } texcoord; // 纹理坐标
    //};
    //// create vertex buffer (1 2d triangle at center of screen)
    //Vertex vertices[] =
    //{

    //  { { -1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },   // 左上 (红色)
    //  { {  1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },   // 右上 (绿色)
    //  { { -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },   // 左下 (白色)
    //  { {  1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },   // 右下 (蓝色)
    //};


    //wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
    //D3D11_BUFFER_DESC bd = {};
    //bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //bd.Usage = D3D11_USAGE_DEFAULT;
    //bd.CPUAccessFlags = 0u;
    //bd.MiscFlags = 0u;
    //bd.ByteWidth = sizeof(vertices);
    //bd.StructureByteStride = sizeof(Vertex);
    //D3D11_SUBRESOURCE_DATA sdata = {};
    //sdata.pSysMem = vertices;
    //pDevice->CreateBuffer(&bd, &sdata, &pVertexBuffer);
    //// Bind vertex buffer to pipeline
    //const UINT stride = sizeof(Vertex);
    //const UINT offset = 0u;
    //pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

    //// create index buffer
    //const unsigned short indices[] =
    //{
    //  0,2,1, 2,3,1,
    //  1,3,5, 3,7,5,
    //  2,6,3, 3,6,7,
    //  4,5,7, 4,7,6,
    //  0,4,2, 2,4,6,
    //  0,1,4, 1,5,4
    //};
    //wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
    //D3D11_BUFFER_DESC ibd = {};
    //ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    //ibd.Usage = D3D11_USAGE_DEFAULT;
    //ibd.CPUAccessFlags = 0u;
    //ibd.MiscFlags = 0u;
    //ibd.ByteWidth = sizeof(indices);
    //ibd.StructureByteStride = sizeof(unsigned short);
    //D3D11_SUBRESOURCE_DATA isd = {};
    //isd.pSysMem = indices;
    //pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);
    //// bind index buffer
    //pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

    //// create constant buffer for transformation matrix
    //struct ConstantBuffer
    //{
    //  dx::XMMATRIX transform;;
    //};
    //const ConstantBuffer cb =
    //{
    //  {
    //      dx::XMMatrixTranspose(
    //          dx::XMMatrixRotationZ(angle) *
    //          dx::XMMatrixRotationX(angle) *
    //          dx::XMMatrixTranslation(x,0.0f,z + 4.0f) *
    //          dx::XMMatrixPerspectiveLH(1.0f,3.0f / 4.0f,0.5f,10.0f)
    //      )
    //  }
    //};
    //wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
    //D3D11_BUFFER_DESC cbd;
    //cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //cbd.Usage = D3D11_USAGE_DYNAMIC;
    //cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //cbd.MiscFlags = 0u;
    //cbd.ByteWidth = sizeof(cb);
    //cbd.StructureByteStride = 0u;
    //D3D11_SUBRESOURCE_DATA csd = {};
    //csd.pSysMem = &cb;
    //pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);
    //// bind constant buffer to vertex shader
    //pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());


    //// lookup table for cube face colors
    //struct ConstantBuffer2
    //{
    //  struct
    //  {
    //      float r;
    //      float g;
    //      float b;
    //      float a;
    //  } face_colors[6];
    //};
    //const ConstantBuffer2 cb2 =
    //{
    //  {
    //      {1.0f,0.0f,1.0f},
    //      {1.0f,0.0f,0.0f},
    //      {0.0f,1.0f,0.0f},
    //      {0.0f,0.0f,1.0f},
    //      {1.0f,1.0f,0.0f},
    //      {0.0f,1.0f,1.0f},
    //  }
    //};
    //wrl::ComPtr<ID3D11Buffer> pConstantBuffer2;
    //D3D11_BUFFER_DESC cbd2;
    //cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //cbd2.Usage = D3D11_USAGE_DEFAULT;
    //cbd2.CPUAccessFlags = 0u;
    //cbd2.MiscFlags = 0u;
    //cbd2.ByteWidth = sizeof(cb2);
    //cbd2.StructureByteStride = 0u;
    //D3D11_SUBRESOURCE_DATA csd2 = {};
    //csd2.pSysMem = &cb2;
    //pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2);
    //// bind constant buffer to pixel shader
    //pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());


    //// create pixel shader
    //wrl::ComPtr<ID3D11PixelShader> pPixelShader;
    //wrl::ComPtr<ID3DBlob> pBlob;
    //D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
    //pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
    //// bind pixel shader
    //pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);


    //// create vertex shader
    //wrl::ComPtr<ID3D11VertexShader> pVertexShader;
    //D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
    //pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
    //// bind vertex shader
    //pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

    //// input (vertex) layout (2d position only)
    //wrl::ComPtr<ID3D11InputLayout> pInputLayout;
    //const D3D11_INPUT_ELEMENT_DESC ied[] =
    //{
    //  { "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //  { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    //};
    //pDevice->CreateInputLayout(
    //  ied, (UINT)std::size(ied),
    //  pBlob->GetBufferPointer(),
    //  pBlob->GetBufferSize(),
    //  &pInputLayout
    //);

    //// bind vertex layout
    //pContext->IASetInputLayout(pInputLayout.Get());


    ////bind render target 
    //pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);
    //// Set primitive topology to triangle list (groups of 3 vertices)
    //pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //// configure viewport
    //D3D11_VIEWPORT vp;
    //vp.Width = 2048u;
    //vp.Height = 768u;
    //vp.MinDepth = 0;
    //vp.MaxDepth = 1;
    //vp.TopLeftX = 0;
    //vp.TopLeftY = 0;
    //pContext->RSSetViewports(1u, &vp);

    //pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u);
}
