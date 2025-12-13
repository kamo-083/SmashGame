/**
 * @file   StageResultUI.h
 *
 * @brief  リザルトUIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/UIElement.h"


// クラスの定義 ===============================================================
/**
 * @brief リザルトUI
 */
class StageResultUI :public UIElement
{
	// クラス定数の宣言 -------------------------------------------------
public:
	static constexpr float ANIM_TIME = 0.25f;

	// データメンバの宣言 -----------------------------------------------
private:
	// 有効
	bool m_enable;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageResultUI();

	// デストラクタ
	~StageResultUI();


	// 操作
public:
	// 初期化処理
	void Initialize(ID3D11ShaderResourceView* texture,
		DirectX::SimpleMath::Vector2 texSize,
		DirectX::SimpleMath::Vector2 windowSize);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(RenderContext context) override;

	// 終了処理
	void Finalize() override;

	// 有効化
	void Enable() { m_enable = true; }


	// 取得/設定
public:
	// 有効かどうかを返す
	bool IsEnable() { return m_enable; }

	// 内部実装
private:

};
