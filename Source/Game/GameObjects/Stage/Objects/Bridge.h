/**
 * @file   Bridge.h
 *
 * @brief  橋に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Object/AudioListener.h"
#include "Source/Game/Common/UserResources.h"
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Physics/PhysicsObject.h"
#include "Source/Game/Physics/Collision.h"
#include "Source/Game/Physics/CollisionManager.h"
#include "Source/Game/GameObjects/Enemy/EnemyManager.h"
#include "Source/Game/UI/Elements/Tween/Tween.h"


// クラスの宣言 ===============================================================
class AreaEffect;


// クラスの定義 ===============================================================
/**
 * @brief 橋
 */
class Bridge	: public AudioListener
{
	// クラス定数の宣言 -------------------------------------------------
private:
	// 各辺の長さの半分
	static constexpr DirectX::SimpleMath::Vector3 HALF_LENGTH = { 1.0f, 1.5f, 0.25f };
	// エフェクトの大きさの調整
	static constexpr float EFFECT_LENGTH_ADJUST = 2.5f;
	// 反発係数
	static constexpr float RESTITUTION = 0.5f;
	// アニメーションの再生時間
	static constexpr float ANIMATION_TIME = 2.0f;


	// データメンバの宣言 -----------------------------------------------
private:
	// 根本の座標
	DirectX::SimpleMath::Vector3 m_rootPosition;
	// 橋本体の座標
	DirectX::SimpleMath::Vector3 m_bridgePosition;

	// 橋の傾き
	DirectX::SimpleMath::Quaternion m_angle;

	// 辺の半分の長さ
	DirectX::SimpleMath::Vector3 m_halfLength;

	// 当たり判定
	OBBCollider m_collider;

	// 衝突判定のハンドル
	uint32_t m_collisionHandle;

	// 四角形
	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;

	// モデル
	DirectX::Model* m_model;

	// エフェクト
	std::unique_ptr<AreaEffect> m_effect;

	// トゥイーン関連
	std::unique_ptr<Tween3D> m_tween;
	Tween3D::UIParams m_tweenParams;

	// 降りた状態
	bool m_isDowned;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Bridge(UserResources* pUR);

	// デストラクタ
	~Bridge();


// 操作
public:
	// 初期化処理
	void Initialize(
		ResourceManager* pRM,
		CollisionManager* pCM,
		EnemyManager* pEM,
		const DirectX::SimpleMath::Vector3& position,
		const float angle = 0.0f);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(const RenderContext& context);

	// 終了処理
	void Finalize();


// 取得/設定
public:
	// 当たり判定の取得
	OBBCollider GetCollider() const { return m_collider; }

// 内部実装
private:
	// 橋を倒す方向・軸を計算
	void CalculateBridgeAxis(float angle, DirectX::SimpleMath::Vector3& outAxis);

	// 橋の当たり判定を更新
	void UpdateBridgeCollider();

	// エフェクトの設定
	void SetupEffect();

	// 当たり判定の設定
	void SetupCollider(CollisionManager* pCM, EnemyManager* pEM);

	// アニメーションの設定
	void SetupAnimation(const DirectX::SimpleMath::Vector3& rotAxis);
};
