/**
 * @file   StageResultUI.h
 *
 * @brief  リザルトUIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/UIElement.h"
#include"Source/Game/Data/TextureData.h"
#include"Source/Game/Object/AudioListener.h"

// クラスの宣言 ===============================================================
class NumberRenderer2D;

// クラスの定義 ===============================================================
/**
 * @brief リザルトUI
 */
class StageResultUI :public UIElement
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// アニメーション時間
	static constexpr float ANIM_TIME = 0.25f;

	// クリアタイムの桁数
	static constexpr int TIME_DIGIT = 2;

	// クリアタイムの表示間隔の調査用倍率
	static constexpr float NUMBER_WIDTH_SCALE = 1.5f;

	// テクスチャ群
	struct Textures
	{
		TextureInfo result;
		TextureInfo clearTime;
		TextureInfo number;
	};

	// 時間
	struct Time
	{
		int minute = 0;		// 分
		int second = 0;		// 秒
	};

	// データメンバの宣言 -----------------------------------------------
private:
	// 有効
	bool m_enable;

	// 数字描画機能
	std::unique_ptr<NumberRenderer2D> m_number;

	// クリアタイム
	Time m_clearTime;

	// クリアタイムのテキスト画像のポインタ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clearTimeTexture;

	// クリアタイムの表示位置
	DirectX::SimpleMath::Vector2 m_clearTimePosition;

	// 音声再生機能
	AudioListener m_audio;

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageResultUI(AudioManager* pAM);

	// デストラクタ
	~StageResultUI();


	// 操作
public:
	// 初期化処理
	void Initialize(
		const Textures& textures,
		const DirectX::SimpleMath::Vector2& windowSize);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(const RenderContext& context) override;

	// 終了処理
	void Finalize() override;

	// リザルト画面を表示
	void OpenResult();

	// 取得/設定
public:
	// 有効かどうかを返す
	bool IsEnable() const { return m_enable; }

	// クリアタイムの設定
	void SetClearTime(float time);

	// 内部実装
private:

};
