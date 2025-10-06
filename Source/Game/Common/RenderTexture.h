/**
 * @file   RenderTexture.h
 *
 * @brief  RenderTextureに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================



// クラスの定義 ===============================================================
/**
 * @brief RenderTexture
 */
class RenderTexture
{
	// クラス定数の宣言 -------------------------------------------------
public:



	// データメンバの宣言 -----------------------------------------------
private:
	// レンダーターゲットテクスチャ
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderTargetTexrute;

	// レンダーターゲットビュー
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	// シェーダーリソースビュー
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

	// テクスチャの幅
	int m_width;

	// テクスチャの高さ
	int m_height;

	// 通常のレンダーターゲットビュー
	ID3D11RenderTargetView* m_defaultRTV;

	// 通常の深度ステンシルビュー
	ID3D11DepthStencilView* m_defaultDSV;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	RenderTexture();

	// デストラクタ
	~RenderTexture();


// 操作
public:
	// 初期化処理
	bool Initialize(
		ID3D11Device* device,
		int width,
		int height,
		ID3D11RenderTargetView* rtv,
		ID3D11DepthStencilView* dsv);

	// 終了処理
	void Finalize();

	void SetRTVTexture(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencilView);

	void SetRTVDefault(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencilView);

	void Clear(ID3D11DeviceContext* context, const float color[4]);


// 取得/設定
public:
	// シェーダーリソースビューのポインタを取得
	ID3D11ShaderResourceView* GetSRV() { return m_shaderResourceView.Get(); }

	// サイズを取得
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

// 内部実装
private:

};
