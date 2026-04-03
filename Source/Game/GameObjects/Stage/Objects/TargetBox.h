/**
 * @file   TargetBox.h
 *
 * @brief  的に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include <functional>
#include "Source/Game/Common/UserResources.h"
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Physics/PhysicsObject.h"
#include "Source/Game/Physics/Collision.h"
#include "Source/Game/Physics/CollisionManager.h"
#include "Source/Game/GameObjects/Enemy/EnemyManager.h"
#include "Source/Game/GameObjects/Stage/Objects/Goal.h"
#include "Source/Game/Interface/IState.h"

// クラスの宣言 ===============================================================
class AreaEffect;

// クラスの定義 ===============================================================
/**
 * @brief 的
 */
class TargetBox
{
	// クラス定数の宣言 -------------------------------------------------
private:
	// 各辺の長さの半分
	static constexpr DirectX::SimpleMath::Vector3 HALF_LENGTH = { 0.5f,0.5f,0.5f };

	// データメンバの宣言 -----------------------------------------------
private:
	// 座標
	DirectX::SimpleMath::Vector3 m_position;

	// 傾き
	DirectX::SimpleMath::Vector3 m_angle;

	// 辺の半分の長さ
	DirectX::SimpleMath::Vector3 m_halfLength;

	// 当たり判定
	OBBCollider m_collider;

	// ぶつかったかどうか
	bool m_isHit;

	// ぶつけた時の処理
	std::function<void()> m_operation;

	// 衝突判定のハンドル
	uint32_t m_collisionHandle;

	// 四角形
	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;

	// モデル
	DirectX::Model* m_model;

	// エフェクト
	std::unique_ptr<AreaEffect> m_effect;

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TargetBox(UserResources* pUR);

	// デストラクタ
	~TargetBox();

// 操作
public:
	// 初期化処理
	void Initialize(
		ResourceManager* pRM,
		CollisionManager* pCM,
		EnemyManager* pEM,
		const std::function<void()>& operation,
		const DirectX::SimpleMath::Vector3& position,
		const DirectX::SimpleMath::Vector3& halfLength = HALF_LENGTH,
		const DirectX::SimpleMath::Vector3& angle = DirectX::SimpleMath::Vector3::Zero);

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

};