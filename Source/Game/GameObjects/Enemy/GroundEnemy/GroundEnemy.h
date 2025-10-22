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

	static constexpr float DITECTION_RANGE = 2.5f;	// プレイヤー感知範囲

	static constexpr float RESTITUTION = 0.9f;	// 反発係数


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
		UserResources* pUR,
		EffectManager* pEM);

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
		uint32_t id) override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(RenderContext& context, Imase::DebugFont* debugFont) override;

	// 終了処理
	void Finalize() override;

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
	GroundEnemy_Idle* GetState_Idle() { return m_idlingState.get(); };			// 待機状態のポインタを取得
	GroundEnemy_Walk* GetState_Walk() { return m_walkingState.get(); };			// 移動状態のポインタを取得
	GroundEnemy_Bounce* GetState_Bounce() { return m_bouncingState.get(); };	// 跳ね返り状態のポインタを取得
	GroundEnemy_Attack* GetState_Attack() { return m_attackingState.get(); };	// 攻撃状態のポインタを取得

	PlayerRelationData GetPlayerRelativeData() { return m_playerRelationData; }	// プレイヤーとの位置関係を取得
	SphereCollider* GetAttackCollider() { return &m_attackCollider; }			// 攻撃の当たり判定を取得
	float GetRotY() { return m_rotY; }								// 向きの取得
	void SetRotY(float rot) { m_rotY = rot + DirectX::XM_PIDIV2; }	// 向きの設定
	float GetRadius() { return RADIUS; }							// 半径サイズの取得

	DirectX::Model* GetModel() { return m_model; }				// モデルのポインタの取得
	Animations* GetAnimation() { return m_animations.get(); }	// アニメーションのポインタ群の取得

	EffectManager::TrajectoryParticleData* GetTrajectoryParticle() { return m_trajectory; }	// 軌跡エフェクトのポインタの取得
	EffectManager::CircleParticleData* GetCircleParticle() { return m_circle; }				// 円形エフェクトのポインタの取得

	DirectX::GeometricPrimitive* GetSpherePrimitive() { return m_sphere.get(); }	// デバッグ用球の取得

// 内部実装
private:
	// 地面・壁との反射
	void ReflectOnCollision(DirectX::SimpleMath::Vector3 normal);
};
