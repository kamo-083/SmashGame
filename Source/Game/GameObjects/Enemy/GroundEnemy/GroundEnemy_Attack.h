/**
 * @file   GroundEnemy_Attack.h
 *
 * @brief  地上の敵の攻撃状態に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Interface/IState.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Common/ModelAnimator.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy.h"

// クラスの宣言 ===============================================================
class GroundEnemy;

// クラスの定義 ===============================================================
/**
 * @brief 地上の敵の攻撃状態
 */
class GroundEnemy_Attack :public IState
{
// クラス定数の宣言 -------------------------------------------------
private:
	const float ATTACK_FORCE;	// 攻撃力
	const float ATTACK_TIME;	// 攻撃持続時間
	const float ATTACK_SIZE;	// 攻撃判定サイズ
	const float DISTANCE_RAITO;	// 体から攻撃判定の中心距離を求める際の倍率

	static constexpr float ANIMATION_SPEED = 2.0f;	// アニメーションの再生速度

// データメンバの宣言 -----------------------------------------------
private:
	// 敵本体へのポインタ
	GroundEnemy* m_pGroundEnemy;

	// モデルアニメーター
	std::unique_ptr<ModelAnimator> m_modelAnimator;

	// 状態の種類
	StateType m_stateType;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GroundEnemy_Attack(GroundEnemy* groundEnemy, const EnemyInfoLoader::EnemyInfo& info);

	// デストラクタ
	~GroundEnemy_Attack() = default;

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