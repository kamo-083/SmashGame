/**
 * @file   Goal.h
 *
 * @brief  ゴールに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"Source/Debug/DebugFont.h"
#include"Source/Game/Physics/Collision.h"
#include"Source/Game/Common/RenderContext.h"
#include "Source/Game/UI/Elements/Tween/Tween.h"


// クラスの定義 ===============================================================
class CollisionManager;
class ResourceManager;
class StageScene;


// クラスの定義 ===============================================================
/**
 * @brief ゴール
 */
class Goal
{
	// クラス定数の宣言 -------------------------------------------------
private:
	// モデル群
	struct Models
	{
		DirectX::Model* fishOnTable;	// テーブルとサカナ
		DirectX::Model* cageLid;		// 檻状のフタ
	};

	// 当たり判定のサイズ
	static constexpr float GOAL_HALF_LENGTH = 1.0f;		// ゴール判定
	static constexpr DirectX::SimpleMath::Vector3 TABLE_HALF_SIZE = { 0.75,1.0f,0.75f };	// テーブル本体

	// トゥイーンアニメーション関連
	static constexpr float LID_MOVE_Y = 5.0f;		// フタのモデルが飛ぶ距離
	static constexpr float TWEEN_ANIM_TIME = 1.25f;	// アニメーションの時間


	// データメンバの宣言 -----------------------------------------------
private:
	// シーンへのポインタ
	StageScene* m_pScene;

	// 位置
	DirectX::SimpleMath::Vector3 m_position;

	// 当たり判定
	OBBCollider m_goalCollider;		// ゴール判定用
	OBBCollider m_tableCollider;	// テーブル用

	// デバッグ用
	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;

	// 当たり判定のハンドル
	uint32_t m_collisionHandle;

	// フラグ系
	bool m_canGoal;	// ゴール可能か
	bool m_isGoal;	// ゴールしているか

	// モデル群
	std::unique_ptr<Models> m_models;

	// トゥイーン関連
	std::unique_ptr<Tween3D> m_tweenAnim;
	Tween3D::UIParams m_tweenParam;


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
	void Initialize(
		ResourceManager* pRM,
		CollisionManager* pCM, 
		DirectX::SimpleMath::Vector3 position);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext& context, DebugFont* debugFont);

	// 終了処理
	void Finalize();


// 取得/設定
public:
	OBBCollider GetCollider(){ return m_goalCollider; }		// 当たり判定の取得
	bool IsGoal() { return m_isGoal; }					// ゴールフラグの取得
	bool IsCanGoal() { return m_canGoal; }				// ゴール可能フラグの取得

	// ゴール可能/不可能の設定
	void CanGoal(bool canGoal = true);


// 内部実装
private:

};
