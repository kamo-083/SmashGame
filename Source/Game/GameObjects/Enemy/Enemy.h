/**
 * @file   Enemy.h
 *
 * @brief  敵に関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Common/Collision.h"
#include"Source/Game/Common/PhysicsEngine/PhysicsObject.h"

#include"ImaseLib/DebugFont.h"

// クラスの定義 ===============================================================
/**
 * @brief 敵
 */
class Enemy
{
// クラス定数の宣言 -------------------------------------------------
protected:
	const static float RADIUS;		//半径の大きさ
	const static float SPEED;		//移動速度
	const static float MASS;		//質量[kg]
	const static float MAX_SPEED;	//最高速度


// データメンバの宣言 -----------------------------------------------
protected:
	//座標
	DirectX::SimpleMath::Vector3 m_position;

	//速度
	DirectX::SimpleMath::Vector3 m_velocity;

	//角度
	float m_rotY;

	//地面
	bool m_onGround;

	//攻撃中
	bool m_isAttack;

	//攻撃力
	float m_attackForce;

	//直前に接触した物
	OBBCollider::CollisionType m_collisionType;

	//法線ベクトル
	DirectX::SimpleMath::Vector3 m_surfaceNormal;

	//球のコライダー
	SphereCollider m_collider;

	//物理演算
	std::unique_ptr<PhysicsObject> m_physics;

	// モデル
	DirectX::Model* m_model;

	// リソースマネージャ
	ResourceManager* m_pResourceManager;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Enemy();

	// デストラクタ
	~Enemy();


// 操作
public:
	// 初期化処理
	virtual void Initialize(ResourceManager* pResourceManager) = 0;

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Draw(RenderContext& context, Imase::DebugFont* debugFont) = 0;

	// 終了処理
	virtual void Finalize() = 0;


// 取得/設定
public:
	DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
	void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
	DirectX::SimpleMath::Vector3& GetVelocity() { return m_velocity; }
	void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }
	SphereCollider* GetCollider() { return &m_collider; }
	PhysicsObject* GetPhysics() { return m_physics.get(); }
	float GetMass() { return MASS; }
	bool GetOnGround() { return m_onGround; }
	void SetOnGround(bool onGround) { m_onGround = onGround; }
	bool GetIsAttack() { return m_isAttack; }
	void SetIsAttack(bool isAttack) { m_isAttack = isAttack; }
	float GetAttackForce() { return m_attackForce; }
	void SetAttackForce(float force) { m_attackForce = force; }

// 内部実装
private:

};
