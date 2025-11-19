/**
 * @file   TargetBox.h
 *
 * @brief  的に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Physics/PhysicsObject.h"
#include "Source/Game/Physics/Collision.h"
#include"Source/Game/Physics/CollisionManager.h"
#include"Source/Game/GameObjects/Enemy/EnemyManager.h"
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Interface/IState.h"
#include "Source/Game/GameObjects/Stage/Objects/Goal.h"
#include "GeometricPrimitive.h"


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
	void Initialize(
		CollisionManager* pCM,
		EnemyManager* pEM,
		Goal* goal,
		DirectX::SimpleMath::Vector3 position,
		DirectX::SimpleMath::Vector3 halfLength = HALF_LENGTH,
		DirectX::SimpleMath::Vector3 angle = DirectX::SimpleMath::Vector3::Zero);

	// 描画処理
	void Draw(RenderContext& context);

	// 終了処理
	void Finalize();


// 取得/設定
public:
	// 当たり判定の取得
	OBBCollider GetCollider() { return m_collider; }

// 内部実装
private:

};
