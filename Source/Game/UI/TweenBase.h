/**
 * @file   TweenBase.h
 *
 * @brief  トゥイーンのベースに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================



// クラスの定義 ===============================================================
/**
 * @brief トゥイーンのベース
 */
class TweenBase
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

	// 内部実装
protected:
	float EaseValue(Ease ease, float t);	// イージングの計算
	float ShortestAngle(float delta);		// 最短回転角度を求める
};