/**
 * @file   TrajectoryParticle.h
 *
 * @brief  TrajectoryParticleに関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Effect/IEffectParticle.h"
#include "Source/Game/Effect/ParticleUtility.h"


// クラスの定義 ===============================================================
/**
 * @brief TrajectoryParticle
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
	~TrajectoryParticle();


// 操作
public:
	// 更新処理
	void Update(float elapsedTime, DirectX::SimpleMath::Vector3 pos, bool spawn, bool isRandom);


// 取得/設定
public:

// 内部実装
private:

};
