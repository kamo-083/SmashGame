/**
 * @file   Tween.h
 *
 * @brief  トゥイーンに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/Tween/Easing.h"


// クラスの定義 ===============================================================
/**
 * @brief トゥイーン
 */
template<typename TVec, typename TRot>
class Tween
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// アニメーションのパラメータ
	struct UIParams
	{
		TVec pos;				// 位置(Vector2/Vector3)
		TVec scale;				// 大きさ(Vector2/Vector3)
		TRot rotation;			// 回転(float(ラジアン)/Quaternion)
		float opacity = 1.0f;	// 不透明度
	};

	// トゥイーンに必要なデータ
	struct TweenData
	{
		UIParams start;		// 初期値
		UIParams delta;		// 全体の変化量
		float duration;		// 再生時間
		Easing::EaseType ease;		// 処理
		Easing::PlaybackMode loop;	// 再生方法
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

	// 再生済みフラグ
	bool m_played;

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

	// 再生の終了
	void End();

	// 再生時間のリセット
	void ResetTime();

	// 再生済みフラグのリセット
	void ResetPlayed() { m_played = false; }


// 取得/設定
public:
	// 再生しているかを取得
	bool IsPlaying() const { return  m_playing; }

	// 終了しているかを取得
	bool Finished() const { return m_finished; }

	// 一度でも再生したかを取得
	bool IsPlayed() const { return m_played; }

	// 開始時のパラメータを取得
	UIParams GetStartParams() const { return m_data.start; }

	// トゥイーン情報を取得
	TweenData GetTweenData() const { return m_data; }

	// トゥイーン情報を設定
	void SetTweenData(const TweenData& data) { m_data = data; }

	// イージング済みの進行度を取得
	float GetEasingProgress() const { return Easing::EaseValue(m_data.ease, m_elapsedTime / m_data.duration); }

	// 素の進行度を取得
	float GetLinearProgress() const { return m_elapsedTime / m_data.duration; }


// 内部実装
private:
	// 回転の補間
	void Rotate(float& param, float t);								// 2D
	void Rotate(DirectX::SimpleMath::Quaternion& param, float t);	// 3D

};

// 関数内部を分離したファイルのインクルード
#include"Source/Game/UI/Elements/Tween/Tween.inl"

// 別名
using Tween2D = Tween<DirectX::SimpleMath::Vector2, float>;
using Tween3D = Tween<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Quaternion>;
