#pragma once

#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Common/ResourceManager.h"

enum class StateType
{
	Idle,
	Walk,
	Attack,
	Bounce,
};

class IState
{
public:
	// デストラクタ
	virtual ~IState() = default;
	// 初期化
	virtual void Initialize(ResourceManager* pRM) = 0;
	// 更新
	virtual void Update(const float& elapsedTime) = 0;
	// 描画
	virtual void Render(const RenderContext& context) = 0;
	// 後処理
	virtual void Finalize() = 0;	

	// 状態の種類を取得
	virtual StateType GetStateType() const = 0;
};