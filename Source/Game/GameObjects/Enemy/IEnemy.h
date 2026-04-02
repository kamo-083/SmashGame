/**
 * @file   IEnemy.h
 *
 * @brief  敵の基底クラスに関するヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Interface/IState.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Physics/CollisionManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Physics/Collision.h"
#include"Source/Game/Physics/PhysicsObject.h"
#include"Source/Game/Effect/EffectManager.h"
#include"Source/Game/Data/EnemyInfoLoader.h"
#include"Source/Game/Object/AudioListener.h"
#include"Source/Debug/DebugFont.h"

// クラスの宣言 ===============================================================
class StageScene;

// クラスの定義 ===============================================================
/**
 * @brief 敵の基底クラス
 */
class IEnemy
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// プレイヤーの距離・向き
	struct PlayerRelationData
	{
		DirectX::SimpleMath::Vector3 direction;
		float distance;
	};

protected:
	const float RADIUS;				// 半径の大きさ
	const float SPEED;				// 移動速度
	const float MASS;				// 質量[kg]
	const float MAX_SPEED;			// 最高速度
	const float STATIC_FRICTION;	// 静止摩擦係数
	const float DYNAMIC_FRICTION;	// 動摩擦係数
	const float RESTITUTION;		// 反発係数

	static constexpr float ANGULAR_VELOCITY = 10.0f;	// 角速度
	static constexpr float SCALE = 0.005f;				// モデルのスケール


	// データメンバの宣言 -----------------------------------------------
protected:
	// 現在の状態へのポインタ
	IState* m_currentState;

	// 座標
	DirectX::SimpleMath::Vector3 m_position;

	// 速度
	DirectX::SimpleMath::Vector3 m_velocity;

	// 角度
	float m_rotY;

	// 攻撃中
	bool m_isAttack;

	// 攻撃力
	float m_attackForce;

	// リスポーン位置
	DirectX::SimpleMath::Vector3 m_respawnPos;

	// 落下時のリスポーン有無
	bool m_dropRespawn;

	// 球のコライダー
	SphereCollider m_collider;

	// 物理演算
	std::unique_ptr<PhysicsObject> m_physics;

	// モデル
	DirectX::Model* m_model;

	// リソースマネージャ
	ResourceManager* m_pResourceManager;

	// 当たり判定マネージャー
	CollisionManager* m_pCollisionManager;

	// 衝突判定のハンドル(本体)
	uint32_t m_handleBody;

	// 衝突判定のハンドル(攻撃)
	uint32_t m_handleAttack;

	// 音声再生機能
	AudioListener m_audio;

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	IEnemy(EnemyInfoLoader::EnemyInfo info, AudioManager* pAM)
		:
		RADIUS{ info.radius },
		SPEED{ info.move_speed },
		MASS{ info.mass },
		MAX_SPEED{ info.max_speed },
		STATIC_FRICTION{ info.static_friction },
		DYNAMIC_FRICTION{ info.dynamic_friction },
		RESTITUTION{ info.restitution },
		m_currentState{ nullptr },
		m_rotY{ 0.0f },
		m_isAttack{ false },
		m_attackForce{ 0.0f },
		m_dropRespawn{ false },
		m_model{ nullptr },
		m_pCollisionManager{ nullptr },
		m_pResourceManager{ nullptr },
		m_handleBody{ 0 },
		m_handleAttack{ 0 },
		m_audio{ pAM }
	{}

	// デストラクタ
	virtual ~IEnemy() = default;


// 操作
public:
	// 初期化処理
	virtual void Initialize(
		ResourceManager* pRM,
		CollisionManager* pCollisionManager,
		const DirectX::SimpleMath::Vector3& position,
		const EnemyInfoLoader::EnemyInfo& info,
		const bool& isRespawn,
		uint32_t id) = 0;

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Draw(const RenderContext& context, DebugFont* debugFont) = 0;

	// 終了処理
	virtual void Finalize() = 0;

	// リスポーン
	virtual void Respawn() = 0;

	// プレイヤーとの距離を計算
	virtual void CalculatePlayerRelationData(const DirectX::SimpleMath::Vector3& pos, float radius) = 0;

	// 攻撃の当たり判定の有効設定
	void SetAttackCollisionEnabled(bool enabled) 
	{ m_pCollisionManager->SetEnabled(m_handleAttack, enabled); }


// 取得/設定
public:
	// 現在のステートの種類を取得
	StateType GetStateType() { return m_currentState->GetStateType(); }	
	// 現在のステートへのポインタを取得
	IState* GetNowState() { return m_currentState; }

	// 座標を取得・設定
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	void SetPosition(const DirectX::SimpleMath::Vector3& pos) { m_position = pos; }

	// 移動速度を取得・設定
	DirectX::SimpleMath::Vector3& GetVelocity() { return m_velocity; }
	void SetVelocity(const DirectX::SimpleMath::Vector3& vel) { m_velocity = vel; }

	// 当たり判定を取得
	SphereCollider* GetCollider() { return &m_collider; }

	// 物理演算オブジェクトを取得
	PhysicsObject* GetPhysics() { return m_physics.get(); }	 
	// 質量を取得
	float GetMass()  const { return MASS; }

	// 攻撃中フラグを取得・設定
	bool GetIsAttack() const { return m_isAttack; }
	void SetIsAttack(bool isAttack) { m_isAttack = isAttack; }

	// 攻撃力を取得・設定
	float GetAttackForce() const { return m_attackForce; }
	void SetAttackForce(float force) { m_attackForce = force; }

	// 大きさを取得
	float  GetScale() const { return SCALE; }

	// 半径サイズの取得
	float GetRadius() const { return RADIUS; }

	// 回転の取得・設定
	float GetRotY() const { return m_rotY; }
	void SetRotY(float royY) { m_rotY = royY; }

	// 落下時のリスポーン有無を取得
	bool IsRespawn() const { return m_dropRespawn; }

// 内部実装
private:

};
