/**
 * @file   TargetBox.h
 *
 * @brief  TargetBoxに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/PhysicsEngine/PhysicsObject.h"
#include "Source/Game/Common/Collision.h"
#include"Source/Game/Common/CollisionManager.h"
#include"Source/Game/GameObjects/Enemy/EnemyManager.h"
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Interface/IState.h"
#include "Source/Game/GameObjects/Stage/Objects/Goal.h"
#include "GeometricPrimitive.h"


// クラスの定義 ===============================================================
/**
 * @brief TargetBox
 */
class TargetBox
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

	// ゴールのポインタ
	Goal* m_pGoal;

	// 衝突判定のハンドル
	uint32_t m_collisionHandle;

	// 四角形
	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TargetBox(ID3D11DeviceContext* context);

	// デストラクタ
	~TargetBox();


// 操作
public:
	// 初期化処理
	void Initialize(CollisionManager* pCollisionManager,
					EnemyManager* pEnemyManager,
					Goal* goal,
					DirectX::SimpleMath::Vector3 position,
					DirectX::SimpleMath::Vector3 halfLength = HALF_LENGTH,
					DirectX::SimpleMath::Vector3 angle = DirectX::SimpleMath::Vector3::Zero);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext& context);

	// 終了処理
	void Finalize();

	// 球との当たり判定
	bool DetectCollisionToEnemy(SphereCollider enemy, StateType state);

// 取得/設定
public:
	OBBCollider GetCollider() { return m_collider; }

// 内部実装
private:

};
