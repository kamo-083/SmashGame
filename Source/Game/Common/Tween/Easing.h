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
		Linear,		// 一定の速度

		InSine,		// ゆっくり始まる
		OutSine,	// ゆっくり終わる
		InOutSine,	// ゆっくり始まり、ゆっくり終わる
		
		InQuart,	// 急加速しながら始まる
		OutQuart,	// 急減速しながら終わる
		InOutQuart,	// 急加速してから急減速
		OutInQuart,	// 急減速してから急加速

		OutBack,	// ゴールを少し通り過ぎてから戻る

		OutElastic,	// ゴールでばねのように揺れる
		
		OutBounce,	// ゴールで跳ね返る
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