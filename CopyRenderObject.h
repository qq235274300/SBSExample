#pragma once
#include "DrawableBase.h"
class CopyRenderObject : public DrawableBase<CopyRenderObject>
{
	CopyRenderObject(Graphics &gfx);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};

