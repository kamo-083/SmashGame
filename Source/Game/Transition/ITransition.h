/**
 * @file   ITransition.h
 *
 * @brief  シーン遷移演出に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/ShaderManager.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/Common/RenderContext.h"

// クラスの宣言 ===============================================================
class DeviceResources;

// クラスの定義 ===============================================================
/**
 * @brief シーン遷移演出
 */
class ITransition
{
// クラス定数の宣言 -------------------------------------------------
public:
	// インプットレイアウト
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	// キーとパスを渡す構造体
	struct ResourcesDesc
	{
		std::string key;	// キー
		std::string path;	// ファイルパス
	};

// データメンバの宣言 -----------------------------------------------
protected:
	// ウィンドウサイズ
	DirectX::SimpleMath::Vector2 m_windowSize;

	// 開閉にかける時間
	float m_interval;

	// 進行割合
	float m_rate;

	// 開閉フラグ
	bool m_open;

	// バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;

	// シェーダーの情報
	ShaderManager::VertexShaderEntry* m_vs;
	ShaderManager::PixelShaderEntry* m_ps;
	ShaderManager::GeometryShaderEntry* m_gs;

	// テクスチャのポインタ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ITransition(
		DX::DeviceResources* pDR,
		ShaderManager* pSM,
		ResourceManager* pRM,
		const ResourcesDesc& vs, const ResourcesDesc& ps, const ResourcesDesc& gs, const ResourcesDesc& tex,
		const DirectX::SimpleMath::Vector2& windowSize, float interval);

	// デストラクタ
	~ITransition();

// 操作
public:
	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Draw(const RenderContext& context) = 0;

	// 開く
	void Open();

	// 閉じる
	void Close();

// 取得/設定
public:
	// シェーダーの読み込み
	void LoadShader(
		ShaderManager* pSM,
		const ResourcesDesc& vs, const ResourcesDesc& ps, const ResourcesDesc& gs);

	// テクスチャの読み込み
	void LoadTexture(ResourceManager* pRM, const ResourcesDesc& desc);

	// オープン中かを返す
	bool IsOpen() const { return m_open; }

	// クローズ中かを返す
	bool IsClose() const { return !m_open; }

	// 終了しているかを返す
	bool IsEnd() const;

	// 進行割合を返す
	float GetRate() const;

// 内部実装
private:

};