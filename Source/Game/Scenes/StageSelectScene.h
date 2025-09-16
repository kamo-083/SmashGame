/**
 * @file   StageSelectScene.h
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
#include "Source/Game/UI/Button.h"


// クラスの宣言 ===============================================================



// クラスの定義 ===============================================================
/**
 * @brief テストシーン
 */
class StageSelectScene : public Scene
{
	// クラス定数の宣言 -------------------------------------------------
public:
	const int STAGES;	// ステージ数


	// データメンバの宣言 -----------------------------------------------
private:
	// システム関連



	// オブジェクト関連
	int m_selectNum;	// 選択中のステージ番号

	std::vector<std::unique_ptr<Button>> m_stagePanels;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageSelectScene(SceneManager* pSceneManager, UserResources* pUserResources, int stages);

	// デストラクタ
	~StageSelectScene();


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
	void PanelReset(int panelNum);


};
