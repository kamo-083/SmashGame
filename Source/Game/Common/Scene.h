/**
 * @file   Scene.h
 *
 * @brief  シーンの基底クラスのヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/RenderContext.h"
#include "Source/Game/Common/UserResources.h"
#include"Source/Debug/DebugFont.h"


// クラスの宣言 ===============================================================
class SceneManager;


// クラスの定義 ===============================================================
/**
 * @brief シーンの基底クラス
 */
class Scene
{
// データメンバの宣言 -----------------------------------------------
private:

protected:
	// シーンマネージャのポインタ
	SceneManager* m_sceneManager;

	// ユーザーリソースのポインタ
	UserResources* m_userResources;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Scene(SceneManager* pSceneManager, UserResources* pUserResources);

	// デストラクタ
	virtual ~Scene();


// 操作
public:
	// 初期化処理
	virtual void Initialize() = 0;

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(RenderContext& context, DebugFont* debugFont) = 0;

	// 終了処理
	virtual void Finalize() = 0;

// 取得
public:


// 内部実装
protected:
	// シーンの変更
	void ChangeScene(const std::string& nextSceneName);
};
