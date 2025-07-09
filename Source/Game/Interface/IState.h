#pragma once

#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Common/ResourceManager.h"

class IState
{
public:
	// デストラクタ
	virtual ~IState() = default;
	// 初期化
	virtual void Initialize(ResourceManager* pResourceManager) = 0;
	// 更新
	virtual void Update(const float& elapsedTime) = 0;
	// 描画
	virtual void Render(RenderContext& context) = 0;
	// 後処理
	virtual void Finalize() = 0;	
};