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
	// 軌跡エフェクト
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

	// 円形エフェクト
	struct CircleParticleData
	{
		std::unique_ptr<CircleParticle> effect;		// エフェクト
		DirectX::SimpleMath::Vector3* position;		// 表示位置
		float range;								// 出現範囲
		int num;									// 出現数
		bool random;								// 出現座標にランダム性を持たせるか
		bool horizontal;							// 出現座標を縦または横にする

		// コンストラクタ
		CircleParticleData(std::unique_ptr<CircleParticle> inEffect, 
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
		void Spawn()
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

	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix	 matWorld;
		DirectX::SimpleMath::Matrix	 matView;
		DirectX::SimpleMath::Matrix	 matProj;
		DirectX::SimpleMath::Vector4 Diffuse;
	};

	// データメンバの宣言 -----------------------------------------------
private:
	// ユーザーリソースのポインタ
	DX::DeviceResources* m_pDeviceResources;

	// カメラのポインタ
	Camera* m_pCamera;

	// エフェクトの配列
	std::vector<std::unique_ptr<TrajectoryParticleData>> m_trajectory;
	std::vector<std::unique_ptr<CircleParticleData>> m_circle;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	DirectX::CommonStates* m_states;

	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EffectManager(DX::DeviceResources* deviceResources, DirectX::CommonStates* states);

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
		ID3D11ShaderResourceView* texture, float scale, float life, DirectX::SimpleMath::Color color,
		DirectX::SimpleMath::Vector3* position, bool random);

	// 円形エフェクトの作成
	CircleParticleData* CreateCircle(
		ID3D11ShaderResourceView* texture, float scale, float life, DirectX::SimpleMath::Color color,
		DirectX::SimpleMath::Vector3* position, float range, int num, bool random, bool horizontal);


// 取得/設定
public:
	// カメラの設定
	void SetCamera(Camera* camera) { m_pCamera = camera; }

// 内部実装
private:
	void CreateShader();
};
