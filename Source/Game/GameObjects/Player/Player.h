/**
 * @file   Player.h
 *
 * @brief  Playerに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Interface/IState.h"
#include"Source/Game/Common/UserResources.h"
#include"Source/Game/Common/CollisionManager.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Common/Collision.h"
#include"Source/Game/Common/PhysicsEngine/PhysicsObject.h"
#include"Source/Game/Effect/EffectManager.h"
#include"Source/Game/Data/AttackData.h"
#include"Source/Game/Scenes/StageScene.h"
#include"ImaseLib/DebugFont.h"
#include"Source/Game/GameObjects/Camera.h"
#include"Source/Game/UI/AttackUI.h"
#include"Source/Game/GameObjects/Player/Player_Idle.h"
#include"Source/Game/GameObjects/Player/Player_Walk.h"
#include"Source/Game/GameObjects/Player/Player_AttackBasic.h"
#include"Source/Game/GameObjects/Player/Player_AttackRolling.h"
#include"Source/Game/GameObjects/Player/Player_AttackHeavy.h"


// クラスの宣言 ===============================================================
class StageScene;
class Player_Idle;
class Player_Walk;
class Player_AttackBasic;
class Player_AttackRolling;
class Player_AttackHeavy;
class AttackUI;


// クラスの定義 ===============================================================
/**
 * @brief Player
 */
class Player
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr float RADIUS = 0.5f;					//半径の大きさ
	static constexpr float MAX_SPEED = 7.0f;				//最高速度
	static constexpr float MASS = 8.0f;						//質量
	static constexpr float SCALE = 0.02f;					//スケール
	static constexpr float DYNAMIC_FRICTION_FORCE = 0.5f;	//動摩擦力
	static constexpr float STATIC_FRICTION_FORCE = 1.0f;	//静止摩擦力
	static constexpr float KILL_HEIGHT = -5.0f;				//落下判定をされる高さ
	static constexpr DirectX::SimpleMath::Vector3 START_POS = { 0.0f,0.0f,2.0f };

	struct Animations
	{
		DX::AnimationSDKMESH* idle;
		DX::AnimationSDKMESH* walk;
		DX::AnimationSDKMESH* atk_basic;
		DX::AnimationSDKMESH* atk_rolling;
		DX::AnimationSDKMESH* atk_heavy;
	};

	// データメンバの宣言 -----------------------------------------------
private:
	// シーンへのポインタ
	StageScene* m_pScene;

	// 座標
	DirectX::SimpleMath::Vector3 m_position;

	// 速度
	DirectX::SimpleMath::Vector3 m_velocity;

	// 向き
	float m_rotY;

	// 地面との接触
	bool m_onGround;

	// 攻撃を受けて吹っ飛ばされた
	bool m_isBounce;

	// モデル
	DirectX::Model* m_model;

	// アニメーション
	std::unique_ptr<Animations> m_animations;

	// リソースマネージャ
	ResourceManager* m_pResourceManager;

	// コライダー
	SphereCollider m_collider;

	// 物理
	std::unique_ptr<PhysicsObject> m_physics;

	// 攻撃
	float m_attackForce;
	bool m_isAttack;
	SphereCollider m_attackCollider;
	AttackType m_attackType;

	// 武器UIのポインタ
	AttackUI* m_pAttackUI;

	// 現在の状態
	IState* m_currentState;

	// 待機状態
	std::unique_ptr<Player_Idle> m_idlingState;

	// 歩き状態
	std::unique_ptr<Player_Walk> m_walkingState;

	// 攻撃状態(通常)
	std::unique_ptr<Player_AttackBasic> m_basicAttackingState;

	// 攻撃状態(転がり)
	std::unique_ptr<Player_AttackRolling> m_rollingAttackingState;

	// 攻撃状態(重量)
	std::unique_ptr<Player_AttackHeavy> m_heavyAttackingState;

	//判定表示用の球
	std::unique_ptr<DirectX::GeometricPrimitive> m_sphere;

	// キー操作のモードのポインタ
	bool* m_pKeyMode;

	// コリジョンマネージャー
	CollisionManager* m_pCollisionManager;

	// 衝突判定のハンドル(本体)
	uint32_t m_handleBody;

	// 衝突判定のハンドル(攻撃)
	uint32_t m_handleAttack;

	// 軌跡エフェクト
	EffectManager::TrajectoryParticleData* m_trajectory;

	// 円形エフェクト
	EffectManager::CircleParticleData* m_circle;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Player(
		UserResources* pUserResources,
		EffectManager* pEffectManager,
		StageScene* pScene);

	// デストラクタ
	~Player();


	// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pRM,
					CollisionManager* pCollisionManager,
					DirectX::Keyboard::KeyboardStateTracker* pKbTracker, 
					Camera* pCamera, 
					AttackUI* pAttackUI,
					bool* pKeyMode);

	// 更新処理
	void Update(const float& elapsedTime);

	// 描画処理
	void Draw(RenderContext& context, Imase::DebugFont* debugFont);

	// 終了処理
	void Finalize();

	// 状態遷移
	void ChangeState(IState* newState);

	// 武器変更
	void ChangeAttack(DirectX::Keyboard::KeyboardStateTracker* pKbTracker);

	// 攻撃
	void Attack();

	// リスポーン
	void Respawn();

	// 移動の方向と回転
	DirectX::SimpleMath::Vector3 MoveDirection(DirectX::Keyboard::KeyboardStateTracker* kbTracker,
											   Camera* camera);

	// 移動速度の制限
	void LimitVelocity(DirectX::SimpleMath::Vector3& velocity, float max = MAX_SPEED);

	// 攻撃の当たり判定の有効設定
	void SetAttackCollisionEnabled(bool enabled);

	// 攻撃の連続ヒットの有効設定
	void SetAttackCollisionMultiHit(bool multiHit);

	// 取得/設定
public:
	DirectX::SimpleMath::Vector3& GetPosition() { return m_position; }
	void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
	DirectX::SimpleMath::Vector3& GetVelocity() { return m_velocity; }
	void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }
	float GetRadius() { return RADIUS; }
	float GetMass() { return MASS; }
	float GetRotY() { return m_rotY; }
	void SetRotY(float rotY) { m_rotY = rotY; }
	bool GetOnGround() { return m_onGround; }
	void SetOnGround(bool onGround) { m_onGround = onGround; }
	PhysicsObject* GetPhysics() { return m_physics.get(); }
	SphereCollider* GetCollider() { return &m_collider; };
	SphereCollider* GetAttackCollider() { return &m_attackCollider; }
	float GetAttackForce() { return m_attackForce; }
	void SetAttackForce(float attackForce) { m_attackForce = attackForce; }
	bool GetIsAttack() { return m_isAttack; }
	void SetIsAttack(bool isAttack) { m_isAttack = isAttack; }
	AttackType GetAttackType() { return m_attackType; }
	bool GetIsBounce() { return m_isBounce; }
	void SetIsBounce(bool isBounce) { m_isBounce = isBounce; }
	DirectX::GeometricPrimitive* GetSpherePrimitive() { return m_sphere.get(); }
	float GetScale() { return SCALE; }
	EffectManager::TrajectoryParticleData* GetTrajectoryParticle() { return m_trajectory; }
	EffectManager::CircleParticleData* GetCircleParticle() { return m_circle; }
	Animations* GetAnimation() { return m_animations.get(); }
	float GetKillHeight() { return KILL_HEIGHT; }

	Player_Idle* GetState_Idle() { return m_idlingState.get(); }
	Player_Walk* GetState_Walk() { return m_walkingState.get(); }
	Player_AttackBasic* GetState_AttackBasic() { return m_basicAttackingState.get(); }
	Player_AttackRolling* GetState_AttackRolling() { return m_rollingAttackingState.get(); }
	Player_AttackHeavy* GetState_AttackHeavy() { return m_heavyAttackingState.get(); }

	// 内部実装
private:

};
