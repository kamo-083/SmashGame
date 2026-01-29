/**
 * @file   Player_Walk.h
 *
 * @brief  プレイヤーの移動状態に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Interface/IState.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Common/ModelAnimator.h"
#include"Source/Game/GameObjects/Camera.h"


// クラスの宣言 ===============================================================
class Player;


// クラスの定義 ===============================================================
/**
 * @brief プレイヤーの移動状態
 */
class Player_Walk :public IState
{
	// クラス定数の宣言 -------------------------------------------------
private:
	const float GROUND_SPEED;	// 移動速度(地面)
	const float AIR_SPEED;		// 移動速度(空中)


	// データメンバの宣言 -----------------------------------------------
private:
	// プレイヤー本体へのポインタ
	Player* m_pPlayer;

	// キーボードトラッカーのポインタ
	DirectX::Keyboard::KeyboardStateTracker* m_pKbTracker;

	// モデルアニメーター
	std::unique_ptr<ModelAnimator> m_modelAnimator;

	// カメラのポインタ
	Camera* m_pCamera;

	// 状態の種類
	StateType m_stateType;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Player_Walk(
		Player* player, Camera* camera,
		DirectX::Keyboard::KeyboardStateTracker* kbTracker,
		float groundSpeed, float airSpeed);

	// デストラクタ
	~Player_Walk() = default;


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

// 取得/設定
public:
	// 状態の種類を取得
	StateType GetStateType() const override { return m_stateType; }


// 内部実装
private:

};
