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
	//���ƹ��캯��
	Graphics(const Graphics&) = delete;
	//���Ƹ�ֵ���캯��
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
public:
	void DrawIndexed(UINT count) noexcept;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	//�������
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;
private:
	DirectX::XMMATRIX projection;
public:
	void BeginFrame(float red, float green, float blue)noexcept;
	void EndFrame();
public:
	void EnableImgui()noexcept;
	void DisableImgui()noexcept;
	bool IsImguiEnabled()const noexcept;
	
private:


private:
	DirectX::XMMATRIX camera;
	bool imguiEnabled = true;
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;

};

