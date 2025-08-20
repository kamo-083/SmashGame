/**
 * @file   TestScene.h
 *
 * @brief  テストシーンに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Scene.h"
#include "Source/Game/Common/CollisionManager.h"
#include "Source/Game/GameObjects/Camera.h"
#include "Source/Game/GameObjects/Player/Player.h"
#include "Source/Game/GameObjects/Enemy/EnemyManager.h"
#include "Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy.h"
#include "Source/Game/GameObjects/Stage/Ground.h"
#include "Source/Game/GameObjects/Stage/BounceBox.h"
#include "Source/Game/GameObjects/Stage/TargetBox.h"
#include "Source/Game/GameObjects/Stage/Goal.h"
#include "Source/Game/UI/WeaponUI.h"


// クラスの宣言 ===============================================================
class Camera;
class Player;
class GroundEnemy;
class Ground;
class WeaponUI;


// クラスの定義 ===============================================================
/**
 * @brief テストシーン
 */
class TestScene : public Scene
{
	// クラス定数の宣言 -------------------------------------------------
public:



	// データメンバの宣言 -----------------------------------------------
private:
	// システム関連
	// 切り替え操作のモード true:武器 false:カメラ
	bool m_keyMode;

	// コリジョンマネージャー
	std::unique_ptr<CollisionManager> m_collisionManager;


	// オブジェクト関連
	//カメラ
	std::unique_ptr<Camera> m_camera;

	//プレイヤー
	std::unique_ptr<Player> m_player;

	//敵
	std::unique_ptr<EnemyManager> m_enemy;

	//地面
	std::vector<std::unique_ptr<Ground>> m_grounds;

	//箱
	std::unique_ptr<BounceBox> m_bounceBox;

	//的
	std::unique_ptr<TargetBox> m_targetBox;
	
	//ゴール
	std::unique_ptr<Goal> m_goal;

	//武器UI
	std::unique_ptr<WeaponUI> m_weaponUI;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TestScene(SceneManager* pSceneManager, UserResources* pUserReources);

	// デストラクタ
	~TestScene();


// 操作
public:
	// 初期化処理
	void Initialize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(RenderContext context, Imase::DebugFont* debugFont) override;

	// 終了処理
	void Finalize() override;


// 取得/設定
public:


// 内部実装
private:


};
