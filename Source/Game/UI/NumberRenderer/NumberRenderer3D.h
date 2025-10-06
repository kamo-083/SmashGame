/**
 * @file   NumberRenderer3D.h
 *
 * @brief  NumberRenderer3Dに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/NumberRenderer/INumberRenderer.h"
#include"Source/Game/Common/DeviceResources.h"
#include"Source/Game/Common/RenderTexture.h"


// クラスの定義 ===============================================================
/**
 * @brief NumberRenderer3D
 */
class NumberRenderer3D :public INumberRenderer
{
	// クラス定数の宣言 -------------------------------------------------
public:
	const static DirectX::VertexPositionTexture VERTECES[4];

	// 表示する数値全体の幅
	const float DIGITS_WIDTH;

	// スケール
	const float SCALE;

	// データメンバの宣言 -----------------------------------------------
private:
	// 表示座標
	DirectX::SimpleMath::Vector3 m_position;

	// ビルボード関連
	bool m_isBillboard;
	DirectX::SimpleMath::Matrix m_billboard;

	//入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	//エフェクト
	std::unique_ptr<DirectX::AlphaTestEffect> m_batchEffect;

	//プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// レンダーテクスチャ
	std::unique_ptr<RenderTexture> m_renderTexture;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	NumberRenderer3D(
		DirectX::SimpleMath::Vector2 spriteSize,
		ID3D11ShaderResourceView* texture,
		int digit,
		DX::DeviceResources* deviceResources,
		float boardScale = 1.f);

	// デストラクタ
	~NumberRenderer3D();


	// 操作
public:
	// 初期化処理
	void Initialize(const int& number) override;

	// 描画処理
	void Draw(RenderContext& renderContext) override;

	// 終了処理
	void Finalize() override;

	// ビルボード行列を作成
	void CreateBillboard(
		DirectX::SimpleMath::Vector3 eye,
		DirectX::SimpleMath::Vector3 up
	);


	// 取得/設定
public:
	// 座標を設定
	void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }

	// スケールを取得
	float GetScale() { return SCALE; }

	// 表示する数値を設定
	void SetNumber(int num) { m_number = num; }


	// 内部実装
private:

};