/**
 * @file   Goal.h
 *
 * @brief  地面に関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"ImaseLib/DebugFont.h"
#include"Source/Game/Common/Collision.h"
#include"Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
class CollisionManager;


// クラスの定義 ===============================================================
/**
 * @brief 地面
 */
class Goal
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr float HALF_LENGTH = 1.0f;
	static constexpr DirectX::SimpleMath::Vector3 ANGLE = { 0.0f,0.0f,0.0f };


	// データメンバの宣言 -----------------------------------------------
private:
	DirectX::SimpleMath::Vector3 m_position;

	OBBCollider m_collider;

	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;

	uint32_t m_collisionHandle;

	bool m_canGoal;
	bool m_isGoal;

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Goal(ID3D11DeviceContext* context);

	// デストラクタ
	~Goal();


// 操作
public:
	// 初期化処理
	void Initialize(CollisionManager* pCollisionManager, DirectX::SimpleMath::Vector3 position);

	// 更新処理
	void Update();

	// 描画処理
	void Draw(RenderContext& context, Imase::DebugFont* debugFont);

	// 終了処理
	void Finalize();

	// 当たり判定
	bool DetectCollisionToPlayer(SphereCollider player);

// 取得/設定
public:
	OBBCollider GetCollider();
	bool IsGoal() { return m_isGoal; }
	void CanGoal(bool canGoal = true) { m_canGoal = canGoal; }


// 内部実装
private:


};
