/**
 * @file   ClearConditionsUI.h
 *
 * @brief  クリア条件UIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/UIElement.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Data/TextureData.h"


// クラスの定義 ===============================================================
/**
 * @brief クリア条件UI
 */
class ClearConditionsUI :public UIElement
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// クリア条件の種類
	enum class ConditionsType
	{
		SMASH,		// ぶつける
		PUSH_OUT,	// 追い出す
		GATHER		// 集める
	};

	// アニメーションのパラメータ
	struct AnimationParam
	{
		DirectX::SimpleMath::Vector2 start;	// 開始位置
		DirectX::SimpleMath::Vector2 delta;	// 変化値
	};

	// 設定されているクリア条件の種類
	const ConditionsType CONDITIONS_TYPE;

	// トゥイーンアニメーションの時間
	static constexpr float ANIM_TIME = 2.0f;

	// 画面端の時のサイズ
	static constexpr float TEXT_SIZE_CORNER = 0.75f;
	static constexpr float TEXT_MARGIN_CORNER = 0.5f;


	// データメンバの宣言 -----------------------------------------------
private:
	// 画像テキストの1列分の大きさ
	DirectX::SimpleMath::Vector2 m_textSize;

	// 開始直後のパラメータ
	AnimationParam m_introParam;
	// 画面端へのパラメータ
	AnimationParam m_cornerParam;
	
	// 開始後のアニメーションは再生済みか
	bool m_isIntroPlayed;


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
		const DirectX::SimpleMath::Vector2& windowSize,
		const TextureInfo& textureInfo);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(const RenderContext& context) override;

	// 終了処理
	void Finalize() override;


	// 取得/設定
public:

	// 内部実装
private:

};
