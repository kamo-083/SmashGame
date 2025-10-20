/**
 * @file   StageScene.h
 *
 * @brief  ステージシーンに関するヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Scene.h"
#include "Source/Game/UI/ClearConditionsUI.h"



// クラスの宣言 ===============================================================
class Camera;
class Player;
class EnemyManager;
class StageManager;
class EffectManager;
class CollisionManager;
class Sky;
class UIWidget;
class AttackUI;
class StageResultUI;
class OperationUI;


// クラスの定義 ===============================================================
/**
 * @brief ステージシーン
 */
class StageScene : public Scene
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// シーンの状態
	enum class Overlay
	{
		NONE = -1,
		GAMEPLAY,	// ゲーム
		RESULT,		// リザルト
	};

	const ClearConditionsUI::ConditionsType CLEAR_CONDITIONS;	// クリア条件


	// データメンバの宣言 -----------------------------------------------
private:
	// システム関連
	// 切り替え操作のモード true:攻撃 false:カメラ
	bool m_keyMode;

	// 当たり判定マネージャー
	std::unique_ptr<CollisionManager> m_collisionManager;

	// エフェクトマネージャー
	std::unique_ptr<EffectManager> m_effectManager;

	// オーバーレイ
	Overlay m_overlayMode;


	// オブジェクト関連
	// カメラ
	std::unique_ptr<Camera> m_camera;

	// プレイヤー
	std::unique_ptr<Player> m_player;

	// 敵マネージャー
	std::unique_ptr<EnemyManager> m_enemyManager;

	// ステージマネージャー
	std::unique_ptr<StageManager> m_stageManager;

	// 攻撃UI
	std::unique_ptr<AttackUI> m_attackUI;

	// リザルトUI
	std::unique_ptr<StageResultUI> m_resultUI;

	// ステージクリア条件UI
	std::unique_ptr<ClearConditionsUI> m_conditionsUI;

	// 操作方法UI
	std::unique_ptr<OperationUI> m_cameraUI;

	// スカイドーム
	std::unique_ptr<Sky> m_sky;

	// ステージファイルへのパス
	std::string m_stageFilePath;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageScene(
		SceneManager* pSceneManager, UserResources* pUserResources,
		std::string path, ClearConditionsUI::ConditionsType clearCondition);

	// デストラクタ
	~StageScene();


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
	
	// SEの再生
	void PlaySE(std::string seName);


// 取得/設定
public:


// 内部実装
private:


};
