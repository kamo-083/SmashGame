/**
 * @file   Sky.h
 *
 * @brief  Skyに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Common/UserResources.h"


// クラスの定義 ===============================================================
/**
 * @brief Sky
 */
class Sky
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:
	// スカイドーム
	DirectX::Model* m_skyDome;


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
	void Initialize(UserResources* pUserResources);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(const RenderContext& context);

	// 終了処理
	void Finalize();

// 取得/設定
public:

// 内部実装
private:

};
