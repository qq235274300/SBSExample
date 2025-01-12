#pragma once

#include "ChiliWin.h"
#include <d3d11_1.h>
#include <vector>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "ChiliException.h"

class DX11Graphics
{
public:
    class Exception : public ChiliException
    {
    public:
        Exception(int line, const char *file, HRESULT hr) noexcept;
        const char *what() const noexcept override;
        virtual const char *GetType() const noexcept;
        static std::string TranslateErrorCode(HRESULT hr) noexcept;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;
    private:
        HRESULT hr;
    };
public:
    DX11Graphics() = default;

    void Init_Resource(ID3D11Device *pDevice, HWND _hwnd, HINSTANCE hInstance);


public:

    // Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
   /* Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;

    Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain1;*/

    // Microsoft::WRL::ComPtr<ID3D11Device> pGraphicsDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;

    float angle = 0;
    float x = 0;
    float z = 0;
};
#define CHWND_EXCEPT( hr ) Window::Exception( __LINE__,__FILE__,hr )
