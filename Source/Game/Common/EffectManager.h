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
public:
	struct TrajectoryParticleData
	{
		std::unique_ptr<TrajectoryParticle> effect;		// エフェクト
		DirectX::SimpleMath::Vector3* position;			// 表示位置
		bool spawn;										// 新たに出現させるか
		bool random;									// 出現座標にランダム性を持たせるか

		// コンストラクタ
		TrajectoryParticleData(std::unique_ptr<TrajectoryParticle> inEffect, 
							   DirectX::SimpleMath::Vector3* inPosition,
							   bool inSpawn,
							   bool inRandom)
			: effect(std::move(inEffect))
			, position(inPosition)
			, spawn(inSpawn)
			, random(inRandom)
		{}

		// 出現フラグの設定
		void SetSpawn(bool inSpawn) { spawn = inSpawn; }
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// ユーザーリソースのポインタ
	DX::DeviceResources* m_pDeviceResources;

	// カメラのポインタ
	Camera* m_pCamera;

	std::vector<std::unique_ptr<TrajectoryParticleData>> m_trajectory;

	std::vector<std::unique_ptr<CircleParticle>> m_circle;



	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EffectManager(DX::DeviceResources* deviceResources);

	// デストラクタ
	~EffectManager();


// 操作
public:
	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(DirectX::SimpleMath::Matrix proj);

	// 終了処理
	void Finalize();

	// 軌跡エフェクトの作成
	TrajectoryParticleData* CreateTrajectory(
		const wchar_t* texPath, float scale, float life, DirectX::SimpleMath::Color color,
		DirectX::SimpleMath::Vector3* position, bool random);


// 取得/設定
public:
	// カメラの設定
	void SetCamera(Camera* camera) { m_pCamera = camera; }

// 内部実装
private:

};
