/**
 * @file   BlockTransition.h
 *
 * @brief  シーン遷移演出に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Transition/ITransition.h"
#include "Source/Game/Common/ShaderManager.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/Common/RenderContext.h"

// クラスの宣言 ===============================================================
class DeviceResources;

// クラスの定義 ===============================================================
/**
 * @brief シーン遷移演出
 */
class BlockTransition :public ITransition
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
		float Rate = 0.0f;							// 進行割合
		float Dummy = 0.0f;
	};

// データメンバの宣言 -----------------------------------------------
private:

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	BlockTransition(
		DX::DeviceResources* pDR,
		ShaderManager* pSM,
		ResourceManager* pRM,
		const ResourcesDesc& vs, const ResourcesDesc& ps, const ResourcesDesc& gs, const ResourcesDesc& tex,
		const DirectX::SimpleMath::Vector2& windowSize, float interval);

	// デストラクタ
	~BlockTransition();

// 操作
public:
	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(const RenderContext& context) override;

	// 終了処理
	void Finalize();

// 取得/設定
public:

// 内部実装
private:

};