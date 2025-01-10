#pragma once

#include "ChiliWin.h"
#include <d3d11_1.h>
#include <vector>

#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class DX11Graphics
{
public:
	DX11Graphics() = default;
	
	void Init_Resource(ID3D11Device *pDevice,HWND hwnd);


public:
	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pGraphicsDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;

	float angle = 0;
	float x = 0;
	float z = 0;
};
