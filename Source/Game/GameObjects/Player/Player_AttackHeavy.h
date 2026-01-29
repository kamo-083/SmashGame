/**
 * @file   Player_AttackHeavy.h
 *
 * @brief  プレイヤーの強攻撃状態に関するヘッダファイル
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
 * @brief プレイヤーの強攻撃状態
 */
class Player_AttackHeavy :public IState
{
	// クラス定数の宣言 -------------------------------------------------
private:
	const float ATTACK_TIME;	// 攻撃持続時間
	const float ATTACK_SIZE;	// 攻撃判定サイズ
	const float ATTACK_FORCE;	// 攻撃力
	const float COOL_TIME;		// 反動時間
	static constexpr float COLLIDER_POS_ADJUST = 0.1f;	// 攻撃判定位置の調整

public:
	// 定数設定パラメータ
	struct AttackParam
	{
		float time;
		float size;
		float force;
		float cool;
	};

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
	Player_AttackHeavy(
		Player* Player, DirectX::Keyboard::KeyboardStateTracker* kbTracker,
		const AttackParam& param);

	// デストラクタ
	~Player_AttackHeavy() = default;


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pRM) override;

	// 更新処理
	void Update(const float& elapsedTime) override;

	// 描画処理
	void Render(const RenderContext& context) override;

	// 終了処理
	void Finalize() override;

// 取得/設定
public:
	// 状態の種類を取得
	StateType GetStateType() const override { return m_stateType; }


// 内部実装
private:

};
