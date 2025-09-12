/**
 * @file   SlidePanel.h
 *
 * @brief  SlidePanelに関するヘッダファイル
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
 * @brief SlidePanel
 */
class SlidePanel
{
	// クラス定数の宣言 -------------------------------------------------
public:
	const int STAGE_NUM;

	enum class DIRECTION
	{
		NONE = -1,
		RIGHT,
		LEFT
	};


	// データメンバの宣言 -----------------------------------------------
private:
	std::unique_ptr<UIWidget> m_UI;

	// 前回動かした方向
	DIRECTION m_lastDirection;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SlidePanel(int stage_num);

	// デストラクタ
	~SlidePanel();


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

	void Slide(DIRECTION dir);


// 取得/設定
public:

// 内部実装
private:

};
