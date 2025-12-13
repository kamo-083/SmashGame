/**
 * @file   Ground.h
 *
 * @brief  地面に関するヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Physics/Collision.h"
#include"Source/Game/Physics/CollisionManager.h"
#include"Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
/**
 * @brief 地面
 */
class Ground
{
// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr DirectX::SimpleMath::Vector3 CENTER_POS = { 0.0f,-0.5f,0.0f };	// デフォルト座標
	static constexpr DirectX::SimpleMath::Vector3 HALF_LENGTH = { 5.0f,0.5f,5.0f };	// デフォルトサイズ
	static constexpr DirectX::SimpleMath::Vector3 ANGLE = { 0.0f,0.0f,0.0f };		// デフォルト角度

// データメンバの宣言 -----------------------------------------------
private:
	// 座標
	DirectX::SimpleMath::Vector3 m_position;
	// 大きさ(各辺の長さの半分)
	DirectX::SimpleMath::Vector3 m_halfLength;
	// 角度
	DirectX::SimpleMath::Vector3 m_angle;

	// 当たり判定
	OBBCollider m_collider;

	// 衝突判定のハンドル
	uint32_t m_collisionHandle;

	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;

	// 深度ステンシルステート
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Ground(ID3D11DeviceContext* context, ID3D11DepthStencilState* pDSS);

	// デストラクタ
	~Ground();


// 操作
public:
	// 初期化処理
	void Initialize(
		CollisionManager* pCM,
		DirectX::SimpleMath::Vector3 position = CENTER_POS,
		DirectX::SimpleMath::Vector3 halfLength = HALF_LENGTH,
		DirectX::SimpleMath::Vector3 angle = ANGLE);

	// 描画処理
	void Draw(RenderContext& context);

	// 終了処理
	void Finalize();


// 取得/設定
public:
	// 当たり判定の取得
	OBBCollider GetCollider(){ return m_collider; }

	// 地面の位置と高さを取得
	DirectX::SimpleMath::Vector3 GetHeight() const;

// 内部実装
private:


};
