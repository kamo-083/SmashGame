/**
 * @file   TweenObject.h
 *
 * @brief  Tween使用オブジェクトのベースに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Common/Tween/Tween.h"

// クラスの定義 ===============================================================
/**
 * @brief TweenObject
 */
class TweenObject
{
	// クラス定数の宣言 -------------------------------------------------
private:

	// データメンバの宣言 -----------------------------------------------
protected:
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	// 画像サイズ
	DirectX::SimpleMath::Vector2 m_texSize;

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TweenObject() = default;
	TweenObject(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
		const DirectX::SimpleMath::Vector2& size)
		:
		m_texture{ texture },
		m_texSize{ size }
	{}

	// デストラクタ
	virtual ~TweenObject() = default;

// 操作
public:
	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Draw(const RenderContext& context) = 0;

// 取得/設定
public:
	// テクスチャサイズの取得
	DirectX::SimpleMath::Vector2 GetTexSize() const { return m_texSize; }
	// テクスチャサイズの設定
	void SetTexSize(const DirectX::SimpleMath::Vector2& texSize) { m_texSize = texSize; }
	// テクスチャの設定
	void SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture) { m_texture = texture; }

// 内部実装
private:

};