/**
 * @file   StageSelectScene.h
 *
 * @brief  テストシーンに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Scene.h"


// クラスの宣言 ===============================================================
class Button;
class NumberRenderer2D;


// クラスの定義 ===============================================================
/**
 * @brief テストシーン
 */
class StageSelectScene : public Scene
{
	// クラス定数の宣言 -------------------------------------------------
public:
	const int STAGES;	// ステージ数

	struct Textures
	{
		ID3D11ShaderResourceView* background;	// 背景
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// システム関連
	std::unique_ptr<NumberRenderer2D> m_numberBoard;


	// オブジェクト関連
	std::unique_ptr<Textures> m_textures;

	int m_selectNum;	// 選択中のステージ番号

	// ステージパネル
	std::vector<std::unique_ptr<Button>> m_stagePanels;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageSelectScene(SceneManager* pSM, UserResources* pUR, int stages);

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
	// 指定したステージパネルをリセット
	void PanelReset(int panelNum);


};
