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

	//无法使用reshade 拿到的device创建swapChain 故 直接创建device跟swapChian
	bool CreateDeviceAndSwapChain(HWND hwnd);
	void CreateSRV_forGameRTV(DXGI_FORMAT currentBackBufferFormat, ID3D11Resource *currentBackBuffer);

    void TestCreateSwapChain(HINSTANCE hInstance);
	
    void Init_Resource(ID3D11Device *pDevice, HWND _hwnd, HINSTANCE hInstance);


public:
	
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;

    float angle = 0;
    float x = 0;
    float z = 0;
};
#define CHWND_EXCEPT( hr ) Window::Exception( __LINE__,__FILE__,hr )
