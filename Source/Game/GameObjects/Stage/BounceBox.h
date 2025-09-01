/**
 * @file   BounceBox.h
 *
 * @brief  BounceBoxに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/PhysicsEngine/PhysicsObject.h"
#include "Source/Game/Common/Collision.h"
#include"Source/Game/Common/CollisionManager.h"
#include"Source/Game/GameObjects/Enemy/EnemyManager.h"
#include "Source/Game/Common/RenderContext.h"
#include "GeometricPrimitive.h"

#include"ImaseLib/DebugFont.h"


// クラスの定義 ===============================================================
/**
 * @brief BounceBox
 */
class BounceBox
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr DirectX::SimpleMath::Vector3 HALF_LENGTH = { 0.5f,0.5f,0.5f };
	static constexpr float MASS = 10.0f;

	// データメンバの宣言 -----------------------------------------------
private:
	// 座標
	DirectX::SimpleMath::Vector3 m_position;

	// 速度
	DirectX::SimpleMath::Vector3 m_velocity;

	// 傾き
	DirectX::SimpleMath::Vector3 m_angle;

	// 辺の半分の長さ
	DirectX::SimpleMath::Vector3 m_halfLength;

	// 当たり判定
	OBBCollider m_collider;

	// 物理
	PhysicsObject m_physics;

	// 地面との接触
	bool m_onGround;

	// 衝突判定のハンドル
	uint32_t m_collisionHandle;

	// 四角形
	std::unique_ptr<DirectX::GeometricPrimitive> m_box;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	BounceBox(ID3D11DeviceContext* context);

	// デストラクタ
	~BounceBox();


// 操作
public:
	// 初期化処理
	void Initialize(CollisionManager* pCollisionManager,
					EnemyManager* pEnemyManager,
					DirectX::SimpleMath::Vector3 position,
					DirectX::SimpleMath::Vector3 halfLength = HALF_LENGTH,
					DirectX::SimpleMath::Vector3 angle = DirectX::SimpleMath::Vector3::Zero);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext& context, Imase::DebugFont* debugFont);

	// 終了処理
	void Finalize();

	// 壁・床との当たり判定
	bool DetectCollisionToBox(OBBCollider obb);

	// 攻撃との当たり判定
	bool DetectCollisionToAttack(SphereCollider attack, SphereCollider player, float power);

// 取得/設定
public:
	OBBCollider GetCollider() { return m_collider; }

// 内部実装
private:

};
