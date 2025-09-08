/**
 * @file   Player_AttackHeavy.h
 *
 * @brief  Player_AttackHeavyに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Interface/IState.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Player.h"


// クラスの宣言 ===============================================================
class Player;


// クラスの定義 ===============================================================
/**
 * @brief Player_AttackHeavy
 */
class Player_AttackHeavy :public IState
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr float ATTACK_FORCE = 5000.0f;
	static constexpr float ATTACK_TIME = 1.0f;
	static constexpr float ATTACK_SIZE = 2.0f;

	// データメンバの宣言 -----------------------------------------------
private:
	Player* m_pPlayer;

	// キーボードトラッカーのポインタ
	DirectX::Keyboard::KeyboardStateTracker* m_pKbTracker;

	// モデル
	DirectX::Model* m_model;

	// 攻撃時間
	float m_attackTime;

	// 状態の種類
	StateType m_stateType;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Player_AttackHeavy(Player* Player, DirectX::Keyboard::KeyboardStateTracker* kbTracker);

	// デストラクタ
	~Player_AttackHeavy();


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pResourceManager) override;

	// 更新処理
	void Update(const float& elapsedTime) override;

	// 描画処理
	void Render(RenderContext& context) override;

	// 終了処理
	void Finalize() override;

// 取得/設定
public:
	// 状態の種類を取得
	StateType GetStateType() const override { return m_stateType; }


// 内部実装
private:

};
