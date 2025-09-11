/**
 * @file   Tween.h
 *
 * @brief  Tweenに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================



// クラスの定義 ===============================================================
/**
 * @brief Tween
 */
class Tween
{
	// クラス定数の宣言 -------------------------------------------------
public:
	enum class Ease
	{
		Liner,
		OutQuart,
		OutBack,
		OutElastic,
		OutBounce,
	};

	enum class PlaybackMode
	{
		Once,
		Once_Reverse,
		Repeat,
		PingPong,
	};

	struct UIParams
	{
		DirectX::SimpleMath::Vector2 pos;	// 位置
		DirectX::SimpleMath::Vector2 scale;	// 大きさ
		float rotation;						// 回転(ラジアン)
		float opacity;						// 不透明度
	};

	struct TweenData
	{
		UIParams start;			// 初期値
		UIParams delta;			// 全体の変化量
		float duration;			// 再生時間
		Ease ease;				// 処理
		PlaybackMode loop;		// 再生方法
	};

	// データメンバの宣言 -----------------------------------------------
private:
	// Tween用の情報
	TweenData m_data;

	// 経過時間
	float m_elapsedTime;

	// 再生フラグ
	bool m_playing;

	// 終了フラグ
	bool m_finished;

	// 反転
	bool m_reverse;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Tween(TweenData data);

	// デストラクタ
	~Tween();


// 操作
public:
	// 更新処理
	void Update(float deltaTime, UIParams& params);

	// 終了処理
	void Finalize();

	// 再生
	void Play();

	// 停止
	void Stop();

	// リセット
	void ResetTime();


// 取得/設定
public:
	// 終了しているか
	bool Finished() const { return m_finished; }
	UIParams GetStartParams() const { return m_data.start; }

// 内部実装
private:
	float EaseValue(Ease ease, float t);
	float ShortestAngle(float delta);
};
