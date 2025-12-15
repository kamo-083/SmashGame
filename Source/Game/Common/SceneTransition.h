/**
 * @file   SceneTransition.h
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
class SceneTransition
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// シェーダーに渡す定数バッファ
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix	 matWorld;		// ワールド行列
		DirectX::SimpleMath::Matrix	 matView;		// ビュー行列
		DirectX::SimpleMath::Matrix	 matProj;		// 射影行列
		DirectX::SimpleMath::Vector4 Diffuse;		// 基本色
		DirectX::SimpleMath::Vector2 WindowSize;	// ウィンドウサイズ
		float Rate;									// 進行割合
		float Dummy;
	};

	// インプットレイアウト
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;


	// データメンバの宣言 -----------------------------------------------
private:
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
	ID3D11ShaderResourceView* m_texture;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SceneTransition(
		DX::DeviceResources* pDR,
		ShaderManager* pSM,
		ResourceManager* pRM,
		DirectX::SimpleMath::Vector2 windowSize, float interval);

	// デストラクタ
	~SceneTransition();


// 操作
public:
	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext context);

	// 終了処理
	void Finalize();

	// 開く
	void Open();

	// 閉じる
	void Close();


// 取得/設定
public:
	// シェーダーの読み込み
	void LoadShader(DX::DeviceResources* pDR, ShaderManager* pSM);

	// テクスチャの読み込み
	void LoadTexture(ResourceManager* pRM);

	// オープン中かを返す
	bool IsOpen() { return m_open; }

	// クローズ中かを返す
	bool IsClose() { return !m_open; }

	// 終了しているかを返す
	bool IsEnd();

	// 進行割合を返す
	float GetRate();


// 内部実装
private:

};
