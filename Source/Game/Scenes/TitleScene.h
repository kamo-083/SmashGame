/**
 * @file   TitleScene.h
 *
 * @brief  タイトルシーンに関するヘッダファイル
 **/

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Scene.h"



// クラスの宣言 ===============================================================
class UIWidget;
class Button;


// クラスの定義 ===============================================================
/**
 * @brief タイトルシーン
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

	static constexpr int BUTTONS = 2;	// ボタン数


	// データメンバの宣言 -----------------------------------------------
private:
	// システム関連


	// オブジェクト関連
	// テクスチャ群
	std::unique_ptr<Textures> m_textures;

	// タイトルロゴ
	std::unique_ptr<UIWidget> m_titleLogo;
	// ボタン
	std::vector<std::unique_ptr<Button>> m_buttons;

	// 選択中のボタン
	int m_selectButton;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TitleScene(SceneManager* pSM, UserResources* pUR);

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
	// 指定したボタンのリセット
	void ButtonReset(int buttonNum);

};
