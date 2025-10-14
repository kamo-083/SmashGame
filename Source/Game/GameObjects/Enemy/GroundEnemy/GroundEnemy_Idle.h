/**
 * @file   GroundEnemy_Idle.h
 *
 * @brief  GroundEnemy_Idleに関するヘッダファイル
 *
 * @author 清水まこと
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
 * @brief GroundEnemy_Idle
 */
class GroundEnemy_Idle :public IState
{
	// クラス定数の宣言 -------------------------------------------------
private:

	// データメンバの宣言 -----------------------------------------------
private:
	GroundEnemy* m_pGroundEnemy;

	// モデルアニメーター
	std::unique_ptr<ModelAnimator> m_modelAnimator;

	// 状態の種類
	StateType m_stateType;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GroundEnemy_Idle(GroundEnemy* groundEnemy);

	// デストラクタ
	~GroundEnemy_Idle();


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
