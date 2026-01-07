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
class Background;
class InputHintUI;


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
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> logo;		// ロゴ
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> start;		// ゲーム開始テキスト
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> exit;		// ゲーム終了テキスト
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> key;		// 操作テキスト
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> action;	// 動作テキスト
	};

	static constexpr int BUTTONS = 2;	// ボタン数

	// タイトルロゴの表示位置
	static constexpr float LOGO_POS_Y = 230.0f;
	// 選択肢テキストの表示位置
	static constexpr float TEXT_POS_Y = 500.0f;
	// 選択肢テキストの表示間隔
	static constexpr float TEXT_INTERVAL = 100.0f;
	// 操作テキストの表示位置
	static constexpr DirectX::SimpleMath::Vector2 INPUT_TEXT_POS = { 500,700 };
	// 操作テキストの表示位置の調整値
	static constexpr float INPUT_TEXT_POS_ADJUST = 15.0f;

	// 操作テキストのスケール
	static constexpr float INPUT_TEXT_SCALE = 0.25f;

	// タイトルロゴの画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 LOGO_SIZE = { 640.0f,360.0f };
	// 選択肢テキストの画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 TEXT_SIZE = { 400.0f,67.0f };
	// 操作テキストの1文字のサイズ
	static constexpr long INPUT_TEXT_SIZE = 120;

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
	// テクスチャ群
	std::unique_ptr<Textures> m_textures;

	// オブジェクト関連
	// タイトルロゴ
	std::unique_ptr<UIWidget> m_titleLogo;

	// ボタン
	std::vector<std::unique_ptr<Button>> m_buttons;

	// 選択中のボタン
	int m_selectButton;

	// 背景
	std::unique_ptr<Background> m_background;

	// 操作方法UI
	std::vector < std::unique_ptr<InputHintUI>> m_inputHintUI;


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
	// ボタンの設定
	void SetupBotton(float windowHalfWidth);
	
	// 背景の設定
	void SetupBackground(
		DX::DeviceResources* pDR,
		ShaderManager* pSM,
		ResourceManager* pRM,
		DirectX::SimpleMath::Vector2 windowSize);

	// テクスチャの設定
	void SetupTexture(ResourceManager* pRM);

	// 音声の設定
	void SetupAudio(AudioManager* pAM);

	// 操作UIの設定
	void SetupInputUI();
};
