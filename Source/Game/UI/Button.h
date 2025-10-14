/**
 * @file   Button.h
 *
 * @brief  Buttonに関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/UIElement.h"


// クラスの定義 ===============================================================
/**
 * @brief Button
 */
class Button :public UIElement
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:
	std::function<void()> m_operate;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Button();

	// デストラクタ
	~Button();


	// 操作
public:
	// 初期化処理
	void Initialize(ID3D11ShaderResourceView* texture,
		const Tween::TweenData data,
		DirectX::SimpleMath::Vector2 size,
		std::function<void()> operate = nullptr);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(RenderContext context) override;

	// 終了処理
	void Finalize() override;

	// 押した
	void Press();

	// リセット
	void Reset();


	// 取得/設定
public:

	// 内部実装
private:

};
