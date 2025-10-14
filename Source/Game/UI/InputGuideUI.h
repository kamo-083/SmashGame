/**
 * @file   InputGuideUI.h
 *
 * @brief  InputGuideUIに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Button.h"



// クラスの定義 ===============================================================
/**
 * @brief InputGuideUI
 */
class InputGuideUI
{
	// クラス定数の宣言 -------------------------------------------------
private:
	struct Textures
	{
		ID3D11ShaderResourceView* attack = nullptr;
		ID3D11ShaderResourceView* move = nullptr;
	};



	// データメンバの宣言 -----------------------------------------------
private:
	std::vector<std::unique_ptr<Button>> m_guides;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	InputGuideUI();

	// デストラクタ
	~InputGuideUI();


// 操作
public:
	// 操作ガイドの追加
	void AddGuide(
		ID3D11ShaderResourceView* texture,
		const Tween::TweenData data,
		DirectX::SimpleMath::Vector2 size);

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
