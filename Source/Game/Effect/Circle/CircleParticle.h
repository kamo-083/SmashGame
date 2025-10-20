/**
 * @file   CircleParticle.h
 *
 * @brief  円形エフェクトパーティクルに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Effect/IEffectParticle.h"
#include "Source/Game/Effect/ParticleUtility.h"


// クラスの定義 ===============================================================
/**
 * @brief 円形エフェクトパーティクル
 */
class CircleParticle	:public IEffectParticle
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr float SPAWN_INTERVAL = 0.15f;	// 最小生成間隔


	// データメンバの宣言 -----------------------------------------------
private:



	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CircleParticle();

	// デストラクタ
	~CircleParticle() = default;


// 操作
public:
	// 更新処理
	void Update(float elapsedTime);

	// エフェクトを出現させる
	void SpawnParticleEffect(
		DirectX::SimpleMath::Vector3 pos,
		float range,
		bool isRandom,
		bool isHorizontal,
		int num);

// 取得/設定
public:

// 内部実装
private:

};
