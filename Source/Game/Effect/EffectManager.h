/**
 * @file   EffectManager.h
 *
 * @brief  エフェクトマネージャーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/DeviceResources.h"
#include "Source/Game/GameObjects/Camera.h"
#include "Source/Game/Common/ShaderManager.h"
#include "Source/Game/Effect/Trajectory/TrajectoryParticle.h"
#include "Source/Game/Effect/Circle/CircleParticle.h"


// クラスの定義 ===============================================================
/**
 * @brief エフェクトマネージャー
 */
class EffectManager
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 軌跡エフェクトのデータ
	struct TrajectoryParticleData
	{
		std::unique_ptr<TrajectoryParticle> effect;		// エフェクト
		DirectX::SimpleMath::Vector3* position;			// 表示位置
		bool spawn;										// 新たに出現させるか
		bool random;									// 出現座標にランダム性を持たせるか

		// コンストラクタ
		TrajectoryParticleData(
			std::unique_ptr<TrajectoryParticle> inEffect,
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

	// 円形エフェクトのデータ
	struct CircleParticleData
	{
		std::unique_ptr<CircleParticle> effect;		// エフェクト
		DirectX::SimpleMath::Vector3* position;		// 表示位置
		float range;								// 出現範囲
		int num;									// 出現数
		bool random;								// 出現座標にランダム性を持たせるか
		bool horizontal;							// 出現座標を縦または横にする

		// コンストラクタ
		CircleParticleData(
			std::unique_ptr<CircleParticle> inEffect,
			DirectX::SimpleMath::Vector3* inPosition,
			float inRange,
			int inNum,
			bool inRandom,
			bool inHorizontal)
			: effect(std::move(inEffect))
			, position(inPosition)
			, range(inRange)
			, num(inNum)
			, random(inRandom)
			, horizontal(inHorizontal)
		{}

		// 出現させる
		void Spawn() const
		{
			effect->SpawnParticleEffect(
				*position,
				range,
				random,
				horizontal,
				num
			);
		}
	};

	// インプットレイアウト
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	// シェーダーに渡す定数バッファ
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix	 matWorld;	// ワールド行列
		DirectX::SimpleMath::Matrix	 matView;	// ビュー行列
		DirectX::SimpleMath::Matrix	 matProj;	// 射影行列
		DirectX::SimpleMath::Vector4 Diffuse;	// 基本色
	};

	// データメンバの宣言 -----------------------------------------------
private:
	// ユーザーリソースのポインタ
	DX::DeviceResources* m_pDeviceResources;

	// カメラのポインタ
	Camera* m_pCamera;

	// エフェクトの配列
	std::vector<std::unique_ptr<TrajectoryParticleData>> m_trajectory;	// 軌跡
	std::vector<std::unique_ptr<CircleParticleData>> m_circle;			// 円形

	// バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	// 共通ステート
	DirectX::CommonStates* m_states;

	// シェーダーの情報
	ShaderManager::VertexShaderEntry* m_vs;		// 頂点シェーダー
	ShaderManager::PixelShaderEntry* m_ps;		// ピクセルシェーダー
	ShaderManager::GeometryShaderEntry* m_gs;	// ジオメトリシェーダー


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EffectManager(DX::DeviceResources* deviceResources, DirectX::CommonStates* states);

	// デストラクタ
	~EffectManager();


// 操作
public:
	// シェーダーの読み込み
	void LoadShaders(ShaderManager* shaderManager);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(const DirectX::SimpleMath::Matrix& proj);

	// 終了処理
	void Finalize();

	// 軌跡エフェクトの作成
	TrajectoryParticleData* CreateTrajectory(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, const ParticleUtility::ParticleData& particleData,
		DirectX::SimpleMath::Vector3* position, bool random);

	// 円形エフェクトの作成
	CircleParticleData* CreateCircle(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, const ParticleUtility::ParticleData& particleData,
		DirectX::SimpleMath::Vector3* position, float range, int num, bool random, bool horizontal);


// 取得/設定
public:
	// カメラの設定
	void SetCamera(Camera* camera) { m_pCamera = camera; }

// 内部実装
private:

};
