/**
 * @file   GroundEnemy_Attack.h
 *
 * @brief  GroundEnemy_Attackに関するヘッダファイル
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
#include"Source/Game/Common/ModelAnimator.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy.h"


// クラスの宣言 ===============================================================
class GroundEnemy;


// クラスの定義 ===============================================================
/**
 * @brief GroundEnemy_Attack
 */
class GroundEnemy_Attack :public IState
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr float ATTACK_FORCE = 5000.0f;
	static constexpr float ATTACK_TIME = 1.0f;
	static constexpr float ATTACK_SIZE = 1.0f;


	// データメンバの宣言 -----------------------------------------------
private:
	GroundEnemy* m_pGroundEnemy;

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
	GroundEnemy_Attack(GroundEnemy* groundEnemy);

	// デストラクタ
	~GroundEnemy_Attack();


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
