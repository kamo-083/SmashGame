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
#include"Source/Game/Interface/IState.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Idle.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Walk.h"
#include"Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy_Bounce.h"


// クラスの定義 ===============================================================
class GroundEnemy_Idle;
class GroundEnemy_Walk;
class GroundEnemy_Bounce;


// クラスの定義 ===============================================================
/**
 * @brief 敵
 */
class GroundEnemy	:public Enemy

{
// クラス定数の宣言 -------------------------------------------------
public:
	// プレイヤーの距離・向き
	struct PlayerRelationData
	{
		DirectX::SimpleMath::Vector3 direction;
		float distance;
	};

	static constexpr float DitectionRange = 2.5f;


// データメンバの宣言 -----------------------------------------------
private:
	// 現在の状態
	IState* m_currentState;

	// 待機状態
	std::unique_ptr<GroundEnemy_Idle> m_idlingState;
	// 歩き状態
	std::unique_ptr<GroundEnemy_Walk> m_walkingState;
	// 跳ね返り状態
	std::unique_ptr<GroundEnemy_Bounce> m_bouncingState;

	// プレイヤーの距離・向き
	PlayerRelationData m_playerRelationData;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GroundEnemy();

	// デストラクタ
	~GroundEnemy();


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pResourceManager) override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(RenderContext& context, Imase::DebugFont* debugFont) override;

	// 終了処理
	void Finalize() override;

	// 状態遷移
	void ChangeState(IState* newState);

	// プレイヤーの距離・向きの計算
	void CalculatePlayerRelationData(DirectX::SimpleMath::Vector3 pos,float radius);

	// 当たり判定
	bool DetectCollisionToBox(OBBCollider collider);
	bool DetectCollisionToSphere(SphereCollider collider);

	// 攻撃の当たり判定
	bool DetectCollisionToAttack(SphereCollider collider, float power);

// 取得/設定
public:
	GroundEnemy_Idle* GetState_Idle() { return m_idlingState.get(); };
	GroundEnemy_Walk* GetState_Walk() { return m_walkingState.get(); };
	GroundEnemy_Bounce* GetState_Bounce() { return m_bouncingState.get(); };
	PlayerRelationData GetPlayerRelativeData() { return m_playerRelationData; }

// 内部実装
private:


};
