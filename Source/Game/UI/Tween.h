/**
 * @file   Tween.h
 *
 * @brief  トゥイーンに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================



// クラスの定義 ===============================================================
/**
 * @brief トゥイーン
 */
class Tween
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// イージングの種類
	enum class Ease
	{
		Liner,
		InQuart,
		OutQuart,
		OutInQuart,
		OutBack,
		OutElastic,
		OutBounce,
	};

	// 再生方法
	enum class PlaybackMode
	{
		Once,			// 一回だけ
		Once_Reverse,	// 一回だけ(反転)
		Repeat,			// 繰り返す
		PingPong,		// 繰り返す(往復)
	};

	// アニメーションのパラメータ
	struct UIParams
	{
		DirectX::SimpleMath::Vector2 pos;	// 位置
		DirectX::SimpleMath::Vector2 scale;	// 大きさ
		float rotation;						// 回転(ラジアン)
		float opacity;						// 不透明度
	};

	// トゥイーンに必要なデータ
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

	// 累計経過時間
	float m_elapsedTime;

	// 再生フラグ
	bool m_playing;

	// 終了フラグ
	bool m_finished;

	// 反転フラグ
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
	// 終了しているかを取得
	bool Finished() const { return m_finished; }

	// 開始時のパラメータを取得
	UIParams GetStartParams() const { return m_data.start; }

	TweenData GetTweenData() const { return m_data; }		// トゥイーン情報を取得
	void SetTweenData(TweenData data) { m_data = data; }	// トゥイーン情報を設定


// 内部実装
private:
	float EaseValue(Ease ease, float t);	// イージングの計算
	float ShortestAngle(float delta);		// 最短回転角度を求める
};
