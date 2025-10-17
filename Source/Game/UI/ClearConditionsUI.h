/**
 * @file   ClearConditionsUI.h
 *
 * @brief  ClearConditionsUIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/UIElement.h"
#include"Source/Game/Common/ResourceManager.h"


// クラスの定義 ===============================================================
/**
 * @brief ClearConditionsUI
 */
class ClearConditionsUI :public UIElement
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// クリア条件の種類
	enum class ConditionsType
	{
		SMASH,
		PUSH_OUT,
		GATHER
	};

	// 画像テキストの1列分の大きさ
	static constexpr DirectX::SimpleMath::Vector2 TEXT_SIZE = { 300.f,90.f };

	// 設定されているクリア条件の種類
	const ConditionsType CONDITIONS_TYPE;


	// データメンバの宣言 -----------------------------------------------
private:


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ClearConditionsUI(ConditionsType clearCondition);

	// デストラクタ
	~ClearConditionsUI();


	// 操作
public:
	// 初期化処理
	void Initialize(
		DirectX::SimpleMath::Vector2 windowSize,
		ResourceManager* pRM);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(RenderContext context) override;

	// 終了処理
	void Finalize() override;


	// 取得/設定
public:

	// 内部実装
private:

};
