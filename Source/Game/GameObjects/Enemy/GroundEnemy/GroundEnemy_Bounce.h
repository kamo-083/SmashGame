/**
 * @file   GroundEnemy_Bounce.h
 *
 * @brief  GroundEnemy_Bounceに関するヘッダファイル
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
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy.h"


// クラスの宣言 ===============================================================
class GroundEnemy;


// クラスの定義 ===============================================================
/**
 * @brief GroundEnemy_Bounce
 */
class GroundEnemy_Bounce :public IState
{
	// クラス定数の宣言 -------------------------------------------------
private:

	// データメンバの宣言 -----------------------------------------------
private:
	GroundEnemy* m_pGroundEnemy;

	// モデル
	DirectX::Model* m_model;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GroundEnemy_Bounce(GroundEnemy* groundEnemy);

	// デストラクタ
	~GroundEnemy_Bounce();


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

// 内部実装
private:

};
