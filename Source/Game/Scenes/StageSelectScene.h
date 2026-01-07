/**
 * @file   StageSelectScene.h
 *
 * @brief  ステージ選択シーンに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Scene.h"


// クラスの宣言 ===============================================================
class Button;
class NumberRenderer2D;
class RenderTexture;
class Background;
class InputHintUI;


// クラスの定義 ===============================================================
/**
 * @brief ステージ選択シーン
 */
class StageSelectScene : public Scene
{
	// クラス定数の宣言 -------------------------------------------------
public:
	const int STAGES;	// ステージ数

	struct Textures
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> key;		// 操作テキスト
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> action;	// 動作テキスト
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stamp_on;	// スタンプ(済)
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stamp_off;	// スタンプ(空)
		std::vector <std::unique_ptr<RenderTexture>> stagePanels;	// ステージパネル
	};

	// スプライト数字の1文字分のサイズ
	static constexpr DirectX::SimpleMath::Vector2 NUMBER_SIZE = { 48.0f,72.0f };

	// パネルの間隔調整
	static constexpr float PANEL_ADJUST_INTERVAL = 210.0f;
	// ステージ番号の間隔調整
	static constexpr float NUMBER_ADJUST_INTERVAL = 230.0f;

	// 操作テキストの表示位置
	static constexpr DirectX::SimpleMath::Vector2 INPUT_TEXT_POS = { 450,700 };
	// 操作テキストの表示位置の調整値
	static constexpr float INPUT_TEXT_POS_ADJUST = 15.0f;
	// 操作テキストのスケール
	static constexpr float INPUT_TEXT_SCALE = 0.25f;
	// 操作テキストの1文字のサイズ
	static constexpr long INPUT_TEXT_SIZE = 120;

	// パネル画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 PANEL_TEX_SIZE = { 350.0f,400.0f };

	// スタンプ画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 STAMP_TEX_SIZE = { 250.0f,250.0f };

	// パネルのトゥイーン関連
	// アニメーションの時間
	static constexpr float PANEL_ANIM_TIME = 0.5f;
	// スケールの変化量
	static constexpr float PANEL_DELTA_SCALE = 0.05f;

	// BGMの音量
	static constexpr float BGM_VOLUME = 0.5f;


	// データメンバの宣言 -----------------------------------------------
private:
	// システム関連
	// テクスチャ群
	std::unique_ptr<Textures> m_textures;

	// 選択中のステージ番号
	int m_selectStage;

	// 遷移先のステージ番号(未登録：-1)
	int m_transitionStage;

	// オブジェクト関連
	// 数字表示ボード
	std::unique_ptr<NumberRenderer2D> m_numberBoard;

	// ステージパネル
	std::vector<std::unique_ptr<Button>> m_stagePanels;

	// ステージクリア情報
	std::vector<bool> m_stageCleared;

	// 背景
	std::unique_ptr<Background> m_background;

	// 操作方法UI
	std::vector < std::unique_ptr<InputHintUI>> m_inputHintUI;


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
	void Render(RenderContext context, DebugFont* debugFont) override;

	// 終了処理
	void Finalize() override;


	// 取得/設定
public:


	// 内部実装
private:
	// 指定したステージパネルをリセット
	void ResetPanel(int panelNum);

	// 選択中のステージを切り替え
	void ChangeSelectStage(DirectX::Keyboard::KeyboardStateTracker* kb);

	// シーンを移動
	void TransitionScene(DirectX::Keyboard::KeyboardStateTracker* kb);

	// 初期設定関連
	// ステージパネルの設定
	void SetupPanel(DirectX::SimpleMath::Vector2 windowSize, ResourceManager* pRM);

	// ステージ番号表示の設定
	void SetupNumberBoard(ResourceManager* pRM);

	// 背景の設定
	void SetupBackground(
		DX::DeviceResources* pDR,
		ShaderManager* pSM,
		ResourceManager* pRM,
		DirectX::SimpleMath::Vector2 windowSize);

	// テクスチャの設定
	void SetupTextures(ResourceManager* pRM);

	// 音声の設定
	void SetupAudios(AudioManager* pAM);

	// 操作UIの設定
	void SetupInputUI();
};
