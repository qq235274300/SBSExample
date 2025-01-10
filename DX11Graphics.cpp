#include "DX11Graphics.h"
#include <DirectXMath.h>
#include <dxgi1_2.h>
#include <iostream>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment  (lib,"d3d11.lib")

#pragma comment (lib,"D3DCompiler.lib")

#define MAX_BACKBUF_COUNT	3

void DX11Graphics::Init_Resource(ID3D11Device *pDevice, HWND hwnd)
{
	namespace wrl = Microsoft::WRL;
	HRESULT hr;

	IDXGISwapChain1 *d3d11SwapChain;
	{
		// 获取 DXGI Factory (需要创建 Swap Chain)
		IDXGIFactory2 *dxgiFactory;
		{
			// 使用 Reshade 中提供的 pDevice 获取 IDXGIDevice1 接口
			IDXGIDevice1 *dxgiDevice;
			HRESULT hResult = pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void **)&dxgiDevice);
			if (FAILED(hResult)) {
				MessageBoxA(0, "Failed to get IDXGIDevice1", "Fatal Error", MB_OK);
				return;
			}

			// 获取适配器 (Adapter)
			IDXGIAdapter *dxgiAdapter;
			hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
			dxgiDevice->Release();
			if (FAILED(hResult)) {
				MessageBoxA(0, "Failed to get IDXGIAdapter", "Fatal Error", MB_OK);
				return;
			}

			// 获取 DXGI Factory
			hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&dxgiFactory);
			dxgiAdapter->Release();
			if (FAILED(hResult)) {
				MessageBoxA(0, "Failed to get IDXGIFactory2", "Fatal Error", MB_OK);
				return;
			}
		}

		// 配置 Swap Chain 描述符
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = 0; // 使用窗口的宽度
		swapChainDesc.Height = 0; // 使用窗口的高度
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 设置格式
		swapChainDesc.SampleDesc.Count = 1; // 设置多重采样数量
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 渲染目标
		swapChainDesc.BufferCount = 2; // 双缓冲
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 丢弃
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = 0;

		// 创建 Swap Chain
		HRESULT hResult = dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, hwnd, &swapChainDesc, nullptr, nullptr, &d3d11SwapChain);
		if (FAILED(hResult)) {
			MessageBoxA(0, "CreateSwapChainForHwnd failed", "Fatal Error", MB_OK);
			dxgiFactory->Release();
			return;
		}

		dxgiFactory->Release();
	}

	/*DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 0;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;*/

//	UINT swapCreateFlags = 0u;
//#ifndef NDEBUG
//	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif


	//IDXGISwapChain *d3d11SwapChain;
	//{
	//	// Get DXGI Factory (needed to create Swap Chain)
	//	IDXGIFactory1 *dxgiFactory;
	//	{
	//		IDXGIDevice1 *dxgiDevice;
	//		HRESULT hResult = pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void **)&dxgiDevice);
	//		assert(SUCCEEDED(hResult));

	//		IDXGIAdapter *dxgiAdapter;
	//		hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
	//		assert(SUCCEEDED(hResult));
	//		dxgiDevice->Release();

	//		DXGI_ADAPTER_DESC adapterDesc;
	//		dxgiAdapter->GetDesc(&adapterDesc);

	//		OutputDebugStringA("Graphics Device: ");
	//		OutputDebugStringW(adapterDesc.Description);

	//		hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), (void **)&dxgiFactory);
	//		assert(SUCCEEDED(hResult));
	//		dxgiAdapter->Release();
	//	}

	//	DXGI_SWAP_CHAIN_DESC d3d11SwapChainDesc = {};
	//	d3d11SwapChainDesc.BufferDesc.Width = 0; // use window width
	//	d3d11SwapChainDesc.BufferDesc.Height = 0; // use window height
	//	d3d11SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	//	d3d11SwapChainDesc.SampleDesc.Count = 1;
	//	d3d11SwapChainDesc.SampleDesc.Quality = 0;
	//	d3d11SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//	d3d11SwapChainDesc.BufferCount = 2;
	//	d3d11SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//	d3d11SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//	
	//	d3d11SwapChainDesc.Flags = 0;

	//	HRESULT hResult = dxgiFactory->CreateSwapChain(pDevice, &d3d11SwapChainDesc, &d3d11SwapChain);
	//	assert(SUCCEEDED(hResult));

	//	dxgiFactory->Release();
	//}




	
	//wrl::ComPtr<ID3D11Resource> pBackBuffer;
	//d3d11SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
	//pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);

	//struct Vertex
	//{
	//	struct
	//	{
	//		float x;
	//		float y;
	//		float z;
	//	} pos;
	//	struct
	//	{
	//		float r;
	//		float g;
	//		float b;
	//		float a;
	//	} color; // 颜色数据

	//	struct
	//	{
	//		float u;
	//		float v;
	//	} texcoord; // 纹理坐标
	//};
	//// create vertex buffer (1 2d triangle at center of screen)
	//Vertex vertices[] =
	//{

	//	{ { -1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },   // 左上 (红色)
	//	{ {  1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },   // 右上 (绿色)
	//	{ { -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },   // 左下 (白色)
	//	{ {  1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },   // 右下 (蓝色)
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
	//	0,2,1, 2,3,1,
	//	1,3,5, 3,7,5,
	//	2,6,3, 3,6,7,
	//	4,5,7, 4,7,6,
	//	0,4,2, 2,4,6,
	//	0,1,4, 1,5,4
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
	//	dx::XMMATRIX transform;;
	//};
	//const ConstantBuffer cb =
	//{
	//	{
	//		dx::XMMatrixTranspose(
	//			dx::XMMatrixRotationZ(angle) *
	//			dx::XMMatrixRotationX(angle) *
	//			dx::XMMatrixTranslation(x,0.0f,z + 4.0f) *
	//			dx::XMMatrixPerspectiveLH(1.0f,3.0f / 4.0f,0.5f,10.0f)
	//		)
	//	}
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
	//	struct
	//	{
	//		float r;
	//		float g;
	//		float b;
	//		float a;
	//	} face_colors[6];
	//};
	//const ConstantBuffer2 cb2 =
	//{
	//	{
	//		{1.0f,0.0f,1.0f},
	//		{1.0f,0.0f,0.0f},
	//		{0.0f,1.0f,0.0f},
	//		{0.0f,0.0f,1.0f},
	//		{1.0f,1.0f,0.0f},
	//		{0.0f,1.0f,1.0f},
	//	}
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
	//	{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	//	{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};
	//pDevice->CreateInputLayout(
	//	ied, (UINT)std::size(ied),
	//	pBlob->GetBufferPointer(),
	//	pBlob->GetBufferSize(),
	//	&pInputLayout
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
