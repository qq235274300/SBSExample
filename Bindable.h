#pragma once
#include "Graphics.h"
class Bindable
{
public:
	virtual ~Bindable() = default;
	virtual void Bind(Graphics& gfx)noexcept = 0;
public:
	static ID3D11DeviceContext *GetContext(Graphics &gfx)noexcept;
	static ID3D11Device *GetDevice(Graphics &gfx)noexcept;
};
