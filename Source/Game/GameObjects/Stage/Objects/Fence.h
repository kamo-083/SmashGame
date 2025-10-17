/**
 * @file   Fence.h
 *
 * @brief  柵に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/Collision.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/CollisionManager.h"


// クラスの定義 ===============================================================
/**
 * @brief 柵
 */
class Fence
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr DirectX::SimpleMath::Vector3 CENTER_POS = { 0.0f,0.0f,0.0f };		// 基本の中心座標
	static constexpr DirectX::SimpleMath::Vector3 DEFAULT_SCALE = { 1.0f,1.0f,1.0f };	// 基本の大きさ
	static constexpr DirectX::SimpleMath::Vector3 ANGLE = { 0.0f,0.0f,0.0f };			// 基本の回転角度
	static constexpr DirectX::SimpleMath::Vector3 FENCE_SIZE = { 0.25f,1.0f,1.0f };		// 柵1つ分の大きさ(x=厚さ、y=高さ、z=幅)

	// データメンバの宣言 -----------------------------------------------
private:
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_scale;
	DirectX::SimpleMath::Vector3 m_angle;

	OBBCollider m_collider;

	// 衝突判定のハンドル
	uint32_t m_collisionHandle;

	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;

	// モデル
	DirectX::Model* m_model;

	// 柵の数
	int m_num;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Fence(ID3D11DeviceContext* context);

	// デストラクタ
	~Fence();


// 操作
public:
	// 初期化処理
	void Initialize(
		ResourceManager* pRM,
		CollisionManager* pCM,
		int num,
		DirectX::SimpleMath::Vector3 position = CENTER_POS,
		DirectX::SimpleMath::Vector3 scale = DEFAULT_SCALE,
		DirectX::SimpleMath::Vector3 angle = ANGLE);

	// 更新処理
	void Update();

	// 描画処理
	void Draw(RenderContext& context);

	// 終了処理
	void Finalize();


// 取得/設定
public:
	OBBCollider GetCollider();


// 内部実装
private:


};
