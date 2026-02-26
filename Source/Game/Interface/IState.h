

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/Message/Message.h"


// 状態の種類
enum class StateType
{
	Idle,		// 待機
	Walk,		// 移動
	Attack,		// 攻撃
	Bounce,		// 跳ね返り
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

	// メッセージを処理
	virtual void OnMessage(Message::MessageID messageID)
	{
		UNREFERENCED_PARAMETER(messageID);
	};
};