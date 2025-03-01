#pragma once
#include "ChiliWin.h"

#include <d3d11.h>
#include <vector>

#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

class Graphics
{
	friend class Bindable;

public:
	Graphics(HWND hwnd);
	//复制构造函数
	Graphics(const Graphics&) = delete;
	//复制赋值构造函数
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
public:
	
	//相机矩阵
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

public:
	void BeginFrame(float red, float green, float blue)noexcept;
	void EndFrame();
	void DrawTestTriangle(float angle, float x, float z);
	void DrawIndexed(UINT count) noexcept;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
public:
	void EnableImgui()noexcept;
	void DisableImgui()noexcept;
	bool IsImguiEnabled()const noexcept;
	
private:
	DirectX::XMMATRIX projection;

private:
	DirectX::XMMATRIX camera;
	bool imguiEnabled = true;
public:
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;

};

