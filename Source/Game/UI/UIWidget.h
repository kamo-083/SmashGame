/**
 * @file   UIWidget.h
 *
 * @brief  UIWidgetに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/UI/Tween.h"


// クラスの定義 ===============================================================
/**
 * @brief UIWidget
 */
class UIWidget
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:
	// テクスチャ
	ID3D11ShaderResourceView* m_texture;

	// UIのパラメータ
	Tween::UIParams m_params;

	// 画像サイズ
	DirectX::SimpleMath::Vector2 m_texSize;

	std::unique_ptr<Tween> m_tween;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	UIWidget();

	// デストラクタ
	~UIWidget();


// 操作
public:
	// 初期化処理
	void Initialize(ID3D11ShaderResourceView* texture,
					const Tween::TweenData data,
					DirectX::SimpleMath::Vector2 size,
					bool play = true);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext context);
	void Draw(DirectX::SpriteBatch* spriteBatch,
			  ID3D11ShaderResourceView* texture = nullptr,
			  DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2::Zero,
			  const RECT* rect = nullptr,
			  float rot = FLT_MAX);

	// 終了処理
	void Finalize();

	// Tweenをリセット
	void TweenReset(bool play = true);


// 取得/設定
public:
	Tween* GetTween() { return m_tween.get(); }
	Tween::UIParams GetParam () const { return m_params; }
	Tween::UIParams GetDelta () const { return m_tween->GetTweenData().delta; }
	void SetTexture(ID3D11ShaderResourceView* texture) { m_texture = texture; }

	// 新しいパラメータをセット
	void SetParam(Tween::UIParams start, Tween::UIParams delta);

// 内部実装
private:

};
