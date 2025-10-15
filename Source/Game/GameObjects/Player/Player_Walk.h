/**
 * @file   Player_Walk.h
 *
 * @brief  Player_Walkに関するヘッダファイル
 *
 * @author 清水まこと
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
 * @brief Player_Walk
 */
class Player_Walk :public IState
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr float GROUND_SPEED = 3.0f;		//移動速度(地面)
	static constexpr float AIR_SPEED = 0.5f;		//移動速度(空中)


	// データメンバの宣言 -----------------------------------------------
private:
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
	Player_Walk(Player* player, Camera* camera, DirectX::Keyboard::KeyboardStateTracker* kbTracker);

	// デストラクタ
	~Player_Walk();


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pRM) override;

	// 更新処理
	void Update(const float& elapsedTime) override;

	// 描画処理
	void Render(RenderContext& context) override;

	// 終了処理
	void Finalize() override;

// 取得/設定
public:
	// 状態の種類を取得
	StateType GetStateType() const override { return m_stateType; }


// 内部実装
private:

};
