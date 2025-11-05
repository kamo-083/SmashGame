/**
 * @file   Player.h
 *
 * @brief  プレイヤーに関するヘッダファイル
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
#include"Source/Game/Data/PlayerInfoLoader.h"
#include"Source/Debug/DebugFont.h"
#include"Source/Game/GameObjects/Camera.h"
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
 * @brief プレイヤー
 */
class Player
{
	// クラス定数の宣言 -------------------------------------------------
private:
	const float RADIUS;					//半径の大きさ
	const float MAX_SPEED;				//最高速度
	const float MASS;					//質量
	const float DYNAMIC_FRICTION_FORCE;	//動摩擦力
	const float STATIC_FRICTION_FORCE;	//静止摩擦力
	static constexpr float SCALE = 0.02f;		//スケール
	static constexpr float KILL_HEIGHT = -5.0f;	//落下判定をされる高さ
	static constexpr DirectX::SimpleMath::Vector3 START_POS = { 0.0f,0.0f,2.0f };	// 初期位置

	// エフェクト関連
	static constexpr float TRAJECTORY_SCALE = 0.5f;	// 軌跡エフェクトのスケール
	static constexpr float TRAJECTORY_LIFE = 2.0f;	// 軌跡エフェクトの寿命
	static constexpr float CIRCLE_SCALE = 0.75f;	// 円形エフェクトのスケール
	static constexpr float CIRCLE_LIFE = 1.0f;		// 円形エフェクトの寿命

	// アニメーション群
	struct Animations
	{
		DX::AnimationSDKMESH* idle;			// 待機
		DX::AnimationSDKMESH* walk;			// 移動
		DX::AnimationSDKMESH* atk_basic;	// 通常攻撃
		DX::AnimationSDKMESH* atk_rolling;	// 転がり攻撃
		DX::AnimationSDKMESH* atk_heavy;	// 強攻撃
	};

public:
	// 初期化時の引数
	struct PlayerParams
	{
		ResourceManager* pRM;
		CollisionManager* pCM;
		DirectX::Keyboard::KeyboardStateTracker* pKbTracker;
		Camera* pCamera;
		AttackUI* pAttackUI;
		bool* pKeyMode;
		const PlayerInfoLoader::PlayerInfo& info;
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// シーンへのポインタ
	StageScene* m_pScene;

	// 位置
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

	// 攻撃関連
	float m_attackForce;				// 攻撃力
	bool m_isAttack;					// 攻撃中か
	SphereCollider m_attackCollider;	// 攻撃判定
	AttackType m_attackType;			// 攻撃の種類

	// 攻撃UIのポインタ
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

	// 当たり判定マネージャー
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
		UserResources* pUR,	EffectManager* pEM,
		StageScene* pScene, const PlayerInfoLoader::PlayerInfo& info);

	// デストラクタ
	~Player();


	// 操作
public:
	// 初期化処理
	void Initialize(PlayerParams param);

	// 更新処理
	void Update(const float& elapsedTime);

	// 描画処理
	void Draw(RenderContext& context, DebugFont* debugFont);

	// 終了処理
	void Finalize();

	// 状態遷移
	void ChangeState(IState* newState);

	// 攻撃変更
	void ChangeAttack(DirectX::Keyboard::KeyboardStateTracker* pKbTracker);

	// 攻撃
	void Attack();

	// リスポーン
	void Respawn();

	// 移動の方向と回転
	DirectX::SimpleMath::Vector3 MoveDirection(DirectX::Keyboard::KeyboardStateTracker* kbTracker,
											   Camera* camera);

	// 移動速度の制限
	void LimitVelocity(DirectX::SimpleMath::Vector3& velocity, float max);

	// 攻撃の当たり判定の有効設定
	void SetAttackCollisionEnabled(bool enabled);

	// 攻撃の連続ヒットの有効設定
	void SetAttackCollisionMultiHit(bool multiHit);

	// 取得/設定
public:
	DirectX::SimpleMath::Vector3& GetPosition() { return m_position; }				// 位置の取得
	void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }		// 位置の設定
	DirectX::SimpleMath::Vector3& GetVelocity() { return m_velocity; }				// 移動速度の取得
	void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }		// 移動速度の設定
	float GetRadius() { return RADIUS; }											// 半径サイズの取得
	float GetMass() { return MASS; }												// 質量の取得
	float GetMaxSpeed() { return MAX_SPEED; }										// 最高移動速度の取得
	float GetRotY() { return m_rotY; }												// 向きの取得
	void SetRotY(float rotY) { m_rotY = rotY; }										// 向きの設定
	bool GetOnGround() { return m_onGround; }										// 接地フラグの取得
	void SetOnGround(bool onGround) { m_onGround = onGround; }						// 接地フラグの設定
	PhysicsObject* GetPhysics() { return m_physics.get(); }							// 物理演算オブジェクトの取得
	SphereCollider* GetCollider() { return &m_collider; };							// 当たり判定の取得
	SphereCollider* GetAttackCollider() { return &m_attackCollider; }				// 攻撃判定の取得
	float GetAttackForce() { return m_attackForce; }								// 攻撃力の取得
	void SetAttackForce(float attackForce) { m_attackForce = attackForce; }			// 攻撃力の設定
	bool GetIsAttack() { return m_isAttack; }										// 攻撃中フラグの取得
	void SetIsAttack(bool isAttack) { m_isAttack = isAttack; }						// 攻撃中フラグの設定
	AttackType GetAttackType() { return m_attackType; }								// 攻撃種類の取得
	bool GetIsBounce() { return m_isBounce; }										// 跳ね返りフラグの取得
	void SetIsBounce(bool isBounce) { m_isBounce = isBounce; }						// 跳ね返りフラグの設定
	DirectX::GeometricPrimitive* GetSpherePrimitive() { return m_sphere.get(); }	// デバッグ用球の取得
	float GetScale() { return SCALE; }												// スケールの取得
	EffectManager::TrajectoryParticleData* GetTrajectoryParticle() { return m_trajectory; }	// 軌跡エフェクトの取得
	EffectManager::CircleParticleData* GetCircleParticle() { return m_circle; }				// 円形エフェクトの取得
	Animations* GetAnimation() { return m_animations.get(); }	// アニメーション群の取得
	float GetKillHeight() { return KILL_HEIGHT; }				// 落下判定高度の取得

	Player_Idle* GetState_Idle() { return m_idlingState.get(); }								// 待機状態の取得
	Player_Walk* GetState_Walk() { return m_walkingState.get(); }								// 移動状態の取得
	Player_AttackBasic* GetState_AttackBasic() { return m_basicAttackingState.get(); }			// 通常攻撃状態の取得
	Player_AttackRolling* GetState_AttackRolling() { return m_rollingAttackingState.get(); }	// 転がり攻撃状態の取得
	Player_AttackHeavy* GetState_AttackHeavy() { return m_heavyAttackingState.get(); }			// 強攻撃状態の取得

	// 移動キーが押されているか
	bool PressMoveKey(DirectX::Keyboard::KeyboardStateTracker* kb);

	// 内部実装
private:
	// 敵の攻撃で吹っ飛ぶ
	void SmashEnemyAttack(const uint32_t& handle);

	// エフェクトの設定
	void SetupEffects(EffectManager* pEM, ResourceManager* pRM);

	// 当たり判定の設定
	void SetupCollision(CollisionManager* pCM);

	// モデル・アニメーションの設定
	void SetupModels(ResourceManager* pRM, const PlayerInfoLoader::PlayerInfo& info);

	// 状態の設定
	void SetupState(
		DirectX::Keyboard::KeyboardStateTracker* pKeyboard, Camera* pCamera,
		const PlayerInfoLoader::PlayerInfo& info);
};
