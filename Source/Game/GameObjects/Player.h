/**
 * @file   Player.h
 *
 * @brief  プレイヤーに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Common/Collision.h"
#include"Source/Game/GameObjects/Camera.h"
#include"Source/Game/Common/PhysicsEngine/PhysicsObject.h"


// クラスの定義 ===============================================================
/**
 * @brief プレイヤー
 */
class Player
{
// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr float RADIUS = 0.5f;	//半径の大きさ
	static constexpr float SPEED  = 10.0f;	//移動速度
	static constexpr float MASS   = 1.0f;	//質量

	//リソース群
	struct Resources
	{
		DirectX::Model* model;
	};


// データメンバの宣言 -----------------------------------------------
private:
	//座標
	DirectX::SimpleMath::Vector3 m_position;
	
	//速度
	DirectX::SimpleMath::Vector3 m_velocity;

	//リソースマネージャ
	ResourceManager* m_pResourceManager;

	//リソース群
	Resources m_resources;

	//コライダー
	SphereCollider m_collider;

	//物理
	PhysicsObject m_physics;

	// カメラのポインタ
	Camera* m_pCamera;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Player();

	// デストラクタ
	~Player();


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pResourceManager, Camera* pCamera);

	// 更新処理
	void Update(DirectX::Keyboard::State keyboard, float elapsedTime);

	// 描画処理
	void Draw(RenderContext& context, Imase::DebugFont* debugFont);

	// 終了処理
	void Finalize();

	//衝突判定
	bool DetectCollisionToEnemy(SphereCollider collider);

// 取得/設定
public:
	//座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() { return m_position; };
	DirectX::SimpleMath::Vector3 GetVelocity() { return m_velocity; };

// 内部実装
private:


};
