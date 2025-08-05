/**
 * @file   EffectManager.h
 *
 * @brief  エフェクトマネージャーに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/GameObjects/Camera.h"
#include"Source/Game/Effect/Trajectory/TrajectoryParticle.h"
#include"Source/Game/Effect/Circle/CircleParticle.h"



// クラスの定義 ===============================================================
/**
 * @brief エフェクトマネージャー
 */
class EffectManager
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:
	// カメラのポインタ
	Camera* m_pCamera;

	std::vector<std::unique_ptr<TrajectoryParticle>> m_trajectory;

	std::vector<std::unique_ptr<CircleParticle>> m_circle;



	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EffectManager();

	// デストラクタ
	~EffectManager();


// 操作
public:
	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 終了処理
	void Finalize();

	// 軌跡エフェクトの作成
	TrajectoryParticle* CreateTrajectory();

// 取得/設定
public:

// 内部実装
private:

};
