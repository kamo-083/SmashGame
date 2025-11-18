/**
 * @file   Easing.h
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
class Easing
{
public:
	// イージングの種類
	enum class EaseType
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

public:
	float static EaseValue(EaseType ease, float t);	// イージングの計算
};