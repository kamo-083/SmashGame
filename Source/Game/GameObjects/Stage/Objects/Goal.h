/**
 * @file   Goal.h
 *
 * @brief  地面に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"ImaseLib/DebugFont.h"
#include"Source/Game/Common/Collision.h"
#include"Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
class CollisionManager;
class StageScene;


// クラスの定義 ===============================================================
/**
 * @brief 地面
 */
class Goal
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr float HALF_LENGTH = 1.0f;
	static constexpr DirectX::SimpleMath::Vector3 ANGLE = { 0.0f,0.0f,0.0f };


	// データメンバの宣言 -----------------------------------------------
private:
	StageScene* m_pScene;

	DirectX::SimpleMath::Vector3 m_position;

	OBBCollider m_collider;

	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;

	uint32_t m_collisionHandle;

	bool m_canGoal;
	bool m_isGoal;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Goal(ID3D11DeviceContext* context, StageScene* pScene);

	// デストラクタ
	~Goal();


// 操作
public:
	// 初期化処理
	void Initialize(CollisionManager* pCM, DirectX::SimpleMath::Vector3 position);

	// 更新処理
	void Update();

	// 描画処理
	void Draw(RenderContext& context, Imase::DebugFont* debugFont);

	// 終了処理
	void Finalize();


// 取得/設定
public:
	OBBCollider GetCollider(){ return m_collider; }		// 当たり判定の取得
	bool IsGoal() { return m_isGoal; }					// ゴールフラグの取得
	bool IsCanGoal() { return m_canGoal; }				// ゴール可能フラグの取得

	// ゴール可能/不可能の設定
	void CanGoal(bool canGoal = true);


// 内部実装
private:


};
