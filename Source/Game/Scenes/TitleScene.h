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
		ID3D11ShaderResourceView* key;			// 操作テキスト
		ID3D11ShaderResourceView* background;	// 背景
	};

	static constexpr int BUTTONS = 2;	// ボタン数

	// タイトルロゴの表示位置
	static constexpr float LOGO_POS_Y = 230.0f;
	// 選択肢テキストの表示位置
	static constexpr float TEXT_POS_Y = 500.0f;
	// 選択肢テキストの表示間隔
	static constexpr float TEXT_INTERVAL = 100.0f;
	// 操作テキストの切り取り範囲
	static constexpr RECT KEY_RECT = { 0,0,290,110 };

	// タイトルロゴの画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 LOGO_SIZE = { 640.0f,360.0f };
	// 選択肢テキストの画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 TEXT_SIZE = { 400.0f,67.0f };

	// トゥイーンアニメーション関連
	// スケール変化量
	static constexpr float TWEEN_DELTA_SCALE = 0.1f;
	// アニメーション時間
	static constexpr float TWEEN_ANIM_TIME = 0.5f;

	// BGMの音量
	static constexpr float BGM_VOLUME = 0.5f;


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
	void Render(RenderContext context, DebugFont* debugFont) override;

	// 終了処理
	void Finalize() override;


	// 取得/設定
public:


	// 内部実装
private:
	// 指定したボタンのリセット
	void ButtonReset(int buttonNum);

	// 選択ボタンの変更
	void SelectButtonUp();		// 上へ
	void SelectButtonDown();	// 下へ

	// 初期設定関連
	// テクスチャの設定
	void SetupTexture(ResourceManager* pRM);

	// 音声の設定
	void SetupAudio(AudioManager* pAM);
};
