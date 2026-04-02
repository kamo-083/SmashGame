/**
 * @file   GroundEnemy.h
 *
 * @brief  地上の敵に関するヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/GameObjects/Enemy/IEnemy.h"
#include"Source/Game/Common/UserResources.h"
#include"Source/Game/GameObjects/Camera.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Idle.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Walk.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Bounce.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Attack.h"


// クラスの宣言 ===============================================================
class GroundEnemy_Idle;
class GroundEnemy_Walk;
class GroundEnemy_Bounce;
class GroundEnemy_Attack;


// クラスの定義 ===============================================================
/**
 * @brief 地上の敵
 */
class GroundEnemy	:public IEnemy

{
// クラス定数の宣言 -------------------------------------------------
public:
	// アニメーションのポインタ
	struct Animations
	{
		DX::AnimationSDKMESH* idle;		// 待機
		DX::AnimationSDKMESH* walk;		// 移動
		DX::AnimationSDKMESH* attack;	// 攻撃
	};

	const float ATTACK_DISTANCE;		// 攻撃距離

	static constexpr float DITECTION_RANGE = 2.5f;	// プレイヤー感知範囲

	static constexpr float RESPAWN_POS_HEIGHT = 1.0f;	// リスポーン時の高さ

	// エフェクト関連
	static constexpr float TRAJECTORY_SCALE = 0.5f;	// 軌跡エフェクトのスケール
	static constexpr float TRAJECTORY_LIFE = 2.0f;	// 軌跡エフェクトの寿命
	static constexpr float CIRCLE_SCALE = 0.75f;	// 円形エフェクトのスケール
	static constexpr float CIRCLE_LIFE = 1.0f;		// 円形エフェクトの寿命


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
	GroundEnemy(
		const EnemyInfoLoader::EnemyInfo& info,
		UserResources* pUR,	EffectManager* pEM);

	// デストラクタ
	~GroundEnemy();


// 操作
public:
	// 初期化処理
	void Initialize(
		ResourceManager* pRM,
		CollisionManager* pCM,
		const DirectX::SimpleMath::Vector3& position,
		const EnemyInfoLoader::EnemyInfo& info,
		const bool& isRespawn,
		uint32_t id) override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(const RenderContext& context, DebugFont* debugFont) override;

	// 終了処理
	void Finalize() override;

	// リスポーン
	void Respawn() override;

	// 状態遷移
	void ChangeState(IState* newState);

	// 移動速度の制限
	void LimitVelocity();

	// プレイヤーの距離・向きの計算
	void CalculatePlayerRelationData(const DirectX::SimpleMath::Vector3& pos,float radius);


// 取得/設定
public:
	// 状態のポインタを取得
	GroundEnemy_Idle* GetState_Idle() { return m_idlingState.get(); };			// 待機
	GroundEnemy_Walk* GetState_Walk() { return m_walkingState.get(); };			// 移動
	GroundEnemy_Bounce* GetState_Bounce() { return m_bouncingState.get(); };	// 跳ね返り
	GroundEnemy_Attack* GetState_Attack() { return m_attackingState.get(); };	// 攻撃

	// プレイヤーとの位置関係を取得
	PlayerRelationData GetPlayerRelativeData() const { return m_playerRelationData; }

	// 攻撃の当たり判定を取得
	SphereCollider* GetAttackCollider() { return &m_attackCollider; }
	// 攻撃の距離を取得
	float GetAttackDistance() const { return ATTACK_DISTANCE; }	

	// モデルのポインタの取得
	DirectX::Model* GetModel() { return m_model; }
	// アニメーションのポインタ群の取得
	Animations* GetAnimation() { return m_animations.get(); }

	// エフェクトのポインタを取得
	EffectManager::TrajectoryParticleData* GetTrajectoryParticle() { return m_trajectory; }	// 軌跡
	EffectManager::CircleParticleData* GetCircleParticle() { return m_circle; }				// 円形

	// デバッグ用球の取得
	DirectX::GeometricPrimitive* GetSpherePrimitive() { return m_sphere.get(); }

// 内部実装
private:
	// 攻撃を受けて吹っ飛ぶ
	void SmashPlayerAttack(const SphereCollider& collider, float power);

	// 地面・壁との反射
	void ReflectOnCollision(const DirectX::SimpleMath::Vector3& normal);

	// エフェクトの設定
	void SetupEffects(EffectManager* pEM, ResourceManager* pRM);

	// モデル・アニメーションの設定
	void SetupModels(ResourceManager* pRM, const EnemyInfoLoader::EnemyInfo& info);

	// 当たり判定の設定
	void SetupCollision(CollisionManager* pCM, const uint32_t& id);

	// 状態の設定
	void SetupState(ResourceManager* pRM, const EnemyInfoLoader::EnemyInfo& info);
};
