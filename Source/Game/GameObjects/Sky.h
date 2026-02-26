/**
 * @file   Sky.h
 *
 * @brief  スカイドームに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Common/UserResources.h"


// クラスの定義 ===============================================================
/**
 * @brief スカイドーム
 */
class Sky
{
	// クラス定数の宣言 -------------------------------------------------
private:


	// データメンバの宣言 -----------------------------------------------
private:
	// スカイドームモデル
	DirectX::Model* m_skyDome;

	// 中心座標のポインタ
	DirectX::SimpleMath::Vector3* m_pPosition;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Sky();

	// デストラクタ
	~Sky();


// 操作
public:
	// 初期化処理
	void Initialize(UserResources* pUR);

	// 描画処理
	void Draw(const RenderContext& context);

	// 終了処理
	void Finalize();

// 取得/設定
public:
	// 中心座標のポインタを設定
	void SetPosition(DirectX::SimpleMath::Vector3* pos) { m_pPosition = pos; }

// 内部実装
private:

};
