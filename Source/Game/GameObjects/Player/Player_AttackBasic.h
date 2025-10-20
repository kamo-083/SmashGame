/**
 * @file   Player_AttackBasic.h
 *
 * @brief  プレイヤーの通常攻撃状態に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Interface/IState.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Common/ModelAnimator.h"
#include"Player.h"


// クラスの宣言 ===============================================================
class Player;


// クラスの定義 ===============================================================
/**
 * @brief プレイヤーの通常攻撃状態
 */
class Player_AttackBasic :public IState
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr float ATTACK_FORCE = 3000.0f;	// 攻撃力
	static constexpr float ATTACK_TIME = 1.0f;		// 攻撃持続時間
	static constexpr float ATTACK_SIZE = 2.0f;		// 攻撃判定サイズ

	// データメンバの宣言 -----------------------------------------------
private:
	// プレイヤー本体へのポインタ
	Player* m_pPlayer;

	// キーボードトラッカーのポインタ
	DirectX::Keyboard::KeyboardStateTracker* m_pKbTracker;

	// モデルアニメーター
	std::unique_ptr<ModelAnimator> m_modelAnimator;

	// 攻撃時間
	float m_attackTime;

	// 状態の種類
	StateType m_stateType;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Player_AttackBasic(Player* Player, DirectX::Keyboard::KeyboardStateTracker* kbTracker);

	// デストラクタ
	~Player_AttackBasic() = default;


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pRM) override;

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
