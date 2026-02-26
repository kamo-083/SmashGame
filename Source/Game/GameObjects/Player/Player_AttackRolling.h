/**
 * @file   Player_AttackRolling.h
 *
 * @brief  プレイヤーの転がり攻撃状態に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Interface/IState.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Common/ModelAnimator.h"
#include"Source/Game/GameObjects/Camera.h"
#include"Player.h"


// クラスの宣言 ===============================================================
class Player;


// クラスの定義 ===============================================================
/**
 * @brief プレイヤーの転がり攻撃状態
 */
class Player_AttackRolling :public IState
{
	// クラス定数の宣言 -------------------------------------------------
private:
	const float ATTACK_TIME;	// 攻撃持続時間
	const float ATTACK_SIZE;	// 攻撃判定サイズ
	const float ATTACK_FORCE;	// 攻撃力
	const float GROUND_SPEED;	// 移動速度(地面)
	const float AIR_SPEED;		// 移動速度(空中)
	static constexpr float ANIM_TIME = 1.5f;			// アニメーション1ループの時間
	
public:
	// 定数設定パラメータ
	struct AttackParam
	{
		float time;
		float size;
		float force;
		float groundSpeed;
		float airSpeed;
	};

	// データメンバの宣言 -----------------------------------------------
private:
	// プレイヤー本体へのポインタ
	Player* m_pPlayer;

	// モデルアニメーター
	std::unique_ptr<ModelAnimator> m_modelAnimator;

	// カメラのポインタ
	Camera* m_pCamera;

	// 攻撃時間
	float m_attackTime;

	// 移動にかける力
	DirectX::SimpleMath::Vector3 m_moveForce;

	// 状態の種類
	StateType m_stateType;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Player_AttackRolling(
		Player* Player, Camera* camera,
		const AttackParam& param);

	// デストラクタ
	~Player_AttackRolling() = default;


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pRM) override;

	// 更新処理
	void Update(const float& elapsedTime) override;

	// 描画処理
	void Render(const RenderContext& context) override;

	// 終了処理
	void Finalize() override;

	// メッセージを処理
	void OnMessage(Message::MessageID messageID) override;


// 取得/設定
public:
	// 状態の種類を取得
	StateType GetStateType() const override { return m_stateType; }


// 内部実装
private:

};
