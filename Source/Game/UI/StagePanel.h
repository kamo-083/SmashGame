/**
 * @file   StagePanel.h
 *
 * @brief  StagePanelに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/UIWidget.h"


// クラスの定義 ===============================================================
/**
 * @brief StagePanel
 */
class StagePanel
{
	// クラス定数の宣言 -------------------------------------------------
public:
	const int STAGE_NUM;


	// データメンバの宣言 -----------------------------------------------
private:
	std::unique_ptr<UIWidget> m_UI;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StagePanel(int stage_num);

	// デストラクタ
	~StagePanel();


// 操作
public:
	// 初期化処理
	void Initialize(ID3D11ShaderResourceView* texture,
					DirectX::SimpleMath::Vector2 texSize,
					DirectX::SimpleMath::Vector2 windowSize);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext context);

	// 終了処理
	void Finalize();

// 取得/設定
public:

// 内部実装
private:

};
