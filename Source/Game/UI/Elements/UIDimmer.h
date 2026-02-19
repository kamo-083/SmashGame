/**
 * @file   UIDimmer.h
 *
 * @brief  UIで使用するディマーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/DeviceResources.h"
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/UI/Elements/Tween/Easing.h"


// クラスの定義 ===============================================================
/**
 * @brief UIで使用するディマー (背景を暗くする)
 */
class UIDimmer
{
	// クラス定数の宣言 -------------------------------------------------
private:


	// データメンバの宣言 -----------------------------------------------
private:
	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// 頂点データ
	std::vector<DirectX::VertexPositionColor> m_vertexes;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	UIDimmer(DX::DeviceResources* pDR);

	// デストラクタ
	~UIDimmer();


// 操作
public:
	// 描画処理
	void Draw(const RenderContext& context);


// 取得/設定
public:
	// 不透明度の設定
	void SetOpacity(float opacity);												// 値を直接設定
	void SetOpacity(float time, Easing::EaseType type, float maxOpacity = 1);	// イージングを使用

// 内部実装
private:
	// 頂点データの作成
	void CreateVertexes(const RECT& windowSize, float opacity);

};
