#include "CopyRenderObject.h"

CopyRenderObject::CopyRenderObject(Graphics &gfx)
{
	namespace dx = DirectX;
	if (!IsStaticInitialized())
	{
		
	}
}

void CopyRenderObject::Update(float dt) noexcept
{
}

DirectX::XMMATRIX CopyRenderObject::GetTransformXM() const noexcept
{
	return DirectX::XMMATRIX();
}
