/**
 * @file   TitleScene.h
 *
 * @brief  テストシーンに関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Scene.h"
#include "Source/Game/UI/UIWidget.h"
#include "Source/Game/UI/Button.h"


// クラスの宣言 ===============================================================


// クラスの定義 ===============================================================
/**
 * @brief テストシーン
 */
class TitleScene : public Scene
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 画像の構造体
	struct Textures
	{
		ID3D11ShaderResourceView* logo;			// ロゴ
		ID3D11ShaderResourceView* start;		// ゲーム開始テキスト
		ID3D11ShaderResourceView* exit;			// 終了テキスト
		ID3D11ShaderResourceView* background;	// 背景
	};

	static constexpr int BUTTONS = 2;


	// データメンバの宣言 -----------------------------------------------
private:
	// システム関連



	// オブジェクト関連
	std::unique_ptr<Textures> m_textures;

	std::unique_ptr<UIWidget> m_titleLogo;
	std::vector<std::unique_ptr<Button>> m_buttons;

	// 選択中のボタン
	int m_selectButton;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TitleScene(SceneManager* pSceneManager, UserResources* pUserResources);

	// デストラクタ
	~TitleScene();


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
	void ButtonReset(int buttonNum);

};
