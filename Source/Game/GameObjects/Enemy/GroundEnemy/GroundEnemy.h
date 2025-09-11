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
#include"Source/Game/GameObjects/Enemy/Enemy.h"
#include"Source/Game/Common/UserResources.h"
#include"Source/Game/GameObjects/Camera.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Idle.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Walk.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Bounce.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Attack.h"


// クラスの定義 ===============================================================
class GroundEnemy_Idle;
class GroundEnemy_Walk;
class GroundEnemy_Bounce;
class GroundEnemy_Attack;


// クラスの定義 ===============================================================
/**
 * @brief 敵
 */
class GroundEnemy	:public Enemy

{
// クラス定数の宣言 -------------------------------------------------
public:
	struct Animations
	{
		DX::AnimationSDKMESH* idle;
		DX::AnimationSDKMESH* walk;
		DX::AnimationSDKMESH* attack;
	};

	static constexpr float DitectionRange = 2.5f;


// データメンバの宣言 -----------------------------------------------
private:
	// 待機状態
	std::unique_ptr<GroundEnemy_Idle> m_idlingState;
	// 歩き状態
	std::unique_ptr<GroundEnemy_Walk> m_walkingState;
	// 跳ね返り状態
	std::unique_ptr<GroundEnemy_Bounce> m_bouncingState;
	// 攻撃状態
	std::unique_ptr<GroundEnemy_Attack> m_attackingState;

	// プレイヤーの距離・向き
	PlayerRelationData m_playerRelationData;

	// 攻撃判定
	SphereCollider m_attackCollider;

	// アニメーション
	std::unique_ptr<Animations> m_animations;

	// 軌跡エフェクト
	EffectManager::TrajectoryParticleData* m_trajectory;

	// 円形エフェクト
	EffectManager::CircleParticleData* m_circle;

	//判定表示用の球
	std::unique_ptr<DirectX::GeometricPrimitive> m_sphere;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GroundEnemy(float radius, float speed, float mass, float maxSpeed,
				UserResources* pUserResources,
				EffectManager* pEffectManager);

	// デストラクタ
	~GroundEnemy();


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pResourceManager,
					CollisionManager* pCollisionManager,
					const DirectX::SimpleMath::Vector3& position,
					uint32_t id) override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(RenderContext& context, Imase::DebugFont* debugFont) override;

	// 終了処理
	void Finalize(CollisionManager* pCollisionManager) override;

	// 状態遷移
	void ChangeState(IState* newState);

	// 移動速度の制限
	void LimitVelocity();

	// プレイヤーの距離・向きの計算
	void CalculatePlayerRelationData(DirectX::SimpleMath::Vector3 pos,float radius);

	// 攻撃の当たり判定
	void DetectCollisionToAttack(SphereCollider collider, float power);


// 取得/設定
public:
	GroundEnemy_Idle* GetState_Idle() { return m_idlingState.get(); };
	GroundEnemy_Walk* GetState_Walk() { return m_walkingState.get(); };
	GroundEnemy_Bounce* GetState_Bounce() { return m_bouncingState.get(); };
	GroundEnemy_Attack* GetState_Attack() { return m_attackingState.get(); };

	PlayerRelationData GetPlayerRelativeData() { return m_playerRelationData; }
	SphereCollider* GetAttackCollider() { return &m_attackCollider; }
	float GetRotY() { return m_rotY; }
	void SetRotY(float rot) { m_rotY = rot + XM_PIDIV2; }	//モデルの向きの関係で少し調整
	float GetRadius() { return RADIUS; }

	Animations* GetAnimation() { return m_animations.get(); }

	EffectManager::TrajectoryParticleData* GetTrajectoryParticle() { return m_trajectory; }
	EffectManager::CircleParticleData* GetCircleParticle() { return m_circle; }

	DirectX::GeometricPrimitive* GetSpherePrimitive() { return m_sphere.get(); }

// 内部実装
private:
	// 地面・壁との反射
	void ReflectOnCollision(DirectX::SimpleMath::Vector3 normal);
};
