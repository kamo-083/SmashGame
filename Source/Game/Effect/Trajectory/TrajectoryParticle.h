/**
 * @file   TrajectoryParticle.h
 *
 * @brief  軌跡エフェクトパーティクルに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Effect/IEffectParticle.h"
#include "Source/Game/Effect/ParticleUtility.h"


// クラスの定義 ===============================================================
/**
 * @brief 軌跡エフェクトパーティクル
 */
class TrajectoryParticle :public IEffectParticle
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:



	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TrajectoryParticle();

	// デストラクタ
	~TrajectoryParticle() = default;


// 操作
public:
	// 更新処理
	void Update(float elapsedTime, const DirectX::SimpleMath::Vector3& pos, bool spawn, bool isRandom);


// 取得/設定
public:

// 内部実装
private:

};
