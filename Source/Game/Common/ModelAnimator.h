/**
 * @file   ModelAnimator.h
 *
 * @brief  モデルアニメーターに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/Animation.h"
#include"Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
/**
 * @brief モデルアニメーター
 */
class ModelAnimator
{
	// クラス定数の宣言 -------------------------------------------------
private:
	// ライトの設定値
	struct LightSetting
	{
		DirectX::SimpleMath::Vector3 direction;	// 方向
		float intensity;						// 強さ
	};

	// 環境光の強さ
	static constexpr float AMBIENT_LIGHT_INTENSITY = 0.7f;

	// メインのライト
	static constexpr LightSetting KEY_LIGHT =
	{
		{ 0.2f, -1.0f, -0.3f },
		0.45f
	};
	
	// 補助ライト(左)
	static constexpr LightSetting FILL_LIGHT_LEFT =
	{
		{ -1.0f, -0.5f, -0.2f },
		0.25f
	};
	
	// 補助ライト(右)
	static constexpr LightSetting FILL_LIGHT_RIGHT =
	{
		{ 1.0f, -0.3f, -0.2f },
		0.15f
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// モデル
	DirectX::Model* m_model;

	// アニメーション
	DX::AnimationSDKMESH* m_animation;

	// ボーン配列
	DirectX::ModelBone::TransformArray m_drawBones;

	// 経過時間
	float m_animElapsedTime;

	// アニメーション終了時間
	float m_animEndTime;

	// 再生速度
	float m_playbackSpeed;

	// ループさせるか
	bool m_loop;

	// 再生中か
	bool m_isPlaying;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ModelAnimator(DirectX::Model* model, DX::AnimationSDKMESH* animation);

	// デストラクタ
	~ModelAnimator();


// 操作
public:
	// 初期化処理
	void Initialize(float endTime = FLT_MAX, bool loop = true, float playbackSpeed = 1.0f);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(const RenderContext& context, const DirectX::SimpleMath::Matrix& world);

	// 終了処理
	void Finalize();

	// 再生
	void Play();


// 取得/設定
public:
	// 再生中か
	bool IsPlaying() const { return m_isPlaying; }

// 内部実装
private:

};
