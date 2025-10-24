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
		ID3D11ShaderResourceView* background;	// 背景
	};

	// スプライト数字の1文字分のサイズ
	static constexpr DirectX::SimpleMath::Vector2 NUMBER_SIZE = { 48.0f,72.0f };

	// パネルの間隔調整
	static constexpr float PANEL_ADJUST_INTERVAL = 210.0f;
	// ステージ番号の間隔調整
	static constexpr float NUMBER_ADJUST_INTERVAL = 230.0f;

	// パネル画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 PANEL_TEX_SIZE = { 350.0f,400.0f };

	// パネルのトゥイーン関連
	// アニメーションの時間
	static constexpr float PANEL_ANIM_TIME = 0.5f;
	// スケールの変化量
	static constexpr float PANEL_DELTA_SCALE = 0.05f;


	// データメンバの宣言 -----------------------------------------------
private:
	// システム関連
	std::unique_ptr<NumberRenderer2D> m_numberBoard;


	// オブジェクト関連
	std::unique_ptr<Textures> m_textures;

	// 選択中のステージ番号
	int m_selectNum;	

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
	void Render(RenderContext context, DebugFont* debugFont) override;

	// 終了処理
	void Finalize() override;


	// 取得/設定
public:


	// 内部実装
private:
	// 指定したステージパネルをリセット
	void PanelReset(int panelNum);


};
