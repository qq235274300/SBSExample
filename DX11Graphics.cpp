#include "DX11Graphics.h"
#include <DirectXMath.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <iostream>
#include "reshade.hpp"
#include <sstream>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment  (lib,"d3d11.lib")

#pragma comment (lib,"D3DCompiler.lib")

#define MAX_BACKBUF_COUNT   3





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

void DX11Graphics::Init_Resource(ID3D11Device *pDevice, HWND _hwnd, HINSTANCE hInstance)
{
    namespace wrl = Microsoft::WRL;
    HRESULT hr;
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
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
        swapChainDesc.Width = 0;
        swapChainDesc.Height = 0;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 1;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Flags = createDeviceFlags;
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

        IDXGIDevice *dxgiDevice = nullptr;
        IDXGIAdapter *dxgiAdapter = nullptr;
        IDXGISwapChain1 *SwapChain1 = nullptr;
        HRESULT hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void **>(&dxgiDevice));
        if (FAILED(hr))
        {
            // QueryInterface 失败时，抛出异常或者进行错误处理
            throw std::runtime_error("Failed to retrieve IDXGIDevice from ID3D11Device.");
        }

#define ReleaseObject(object) if((object) != NULL) { object->Release(); object = NULL; }


        if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void **>(&dxgiAdapter))))
        {
            ReleaseObject(dxgiDevice);
            throw std::runtime_error("IDXGIDevice::GetParent() failed retrieving adapter.");
        }

        IDXGIFactory2 *dxgiFactory = nullptr;
        if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void **>(&dxgiFactory))))
        {
            ReleaseObject(dxgiDevice);
            ReleaseObject(dxgiAdapter);
            throw std::runtime_error("IDXGIAdapter::GetParent() failed retrieving factory.");
        }

        if (dxgiFactory)
        {
            dxgiFactory->CreateSwapChainForHwnd(pDevice, _hwnd, &swapChainDesc, nullptr, nullptr, &SwapChain1);
        }

        /* hr = dxgiFactory->CreateSwapChainForHwnd(pDevice, _hwnd, &swapChainDesc, nullptr, nullptr, &pSwapChain1);
         {


         }

         ReleaseObject(dxgiDevice);
         ReleaseObject(dxgiAdapter);
         ReleaseObject(dxgiFactory);*/


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
