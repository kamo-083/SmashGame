/**
 * @file   Easing.cpp
 *
 * @brief  イージングの計算に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Easing.h"

// イージングに使用する数値群
namespace
{
	// Back
	// どれだけ通り過ぎるか
	constexpr float BackOvershoot = 1.70158f;

	// Elastic
	// 減衰計算の底
	constexpr float ElasticAmplitudeBase = 2.0f;
	// 揺れを小さくする速さ
	constexpr float ElasticDecayRate = 10.0f;
	// 揺れの細かさ
	constexpr float ElasticOscillationCount = 10.0f;
	// 揺れの周期を調整する
	constexpr float ElasticPeriodDivisor = 3.0f;
	// 揺れの開始位置をずらす
	constexpr float ElasticPhaseShift = 0.75f;

	// Bounce
	// 跳ね返りの放物線を作る
	constexpr float BounceCoefficient = 7.5625f;
	// 跳ね返り区間を分ける基準値
	constexpr float BounceIntervalScale = 2.75f;
	// 跳ね返り区間でtをずらす
	constexpr float BounceSecondStart = 1.5f;
	constexpr float BounceThirdStart = 2.25f;
	constexpr float BounceFourthStart = 2.625f;
	// 跳ね返り後の高さ補正
	constexpr float BounceSecondHeight = 0.75f;
	constexpr float BounceThirdHeight = 0.9375f;
	constexpr float BounceFourthHeight = 0.984375f;
}

// メンバ関数の定義 ===========================================================
/**
 * @brief イージングの計算
 *
 * @param ease	イージングの種類
 * @param t		進行度
 *
 * @return 進行度
 */
float Easing::EaseValue(EaseType ease, float t)
{
	switch (ease)
	{
	// 一定の速度
	case Easing::EaseType::Linear:
	{
		return t;
	}
	// ゆっくり始まる
	case Easing::EaseType::InSine:
	{
		return 1 - std::cos((t * DirectX::XM_PI) / 2);
	}
	// ゆっくり終わる
	case Easing::EaseType::OutSine:
	{
		return std::cos((t * DirectX::XM_PI) / 2);
	}
	// ゆっくり始まり、ゆっくり終わる
	case Easing::EaseType::InOutSine:
	{
		return -(std::cos(DirectX::XM_PI * t) - 1) / 2;
	}
	// 急加速しながら始まる
	case Easing::EaseType::InQuart:
	{
		return std::pow(t, 4.f);
	}
	// 急減速しながら終わる
	case Easing::EaseType::OutQuart:
	{
		return 1.f - std::pow(1.f - t, 4.f);
	}
	// 急加速してから急減速
	case Easing::EaseType::InOutQuart:
	{
		if (t < 0.5f) {
			// 前半：In
			return 0.5f * EaseValue(EaseType::InQuart, t * 2.f - 1.f);
		}
		else {
			// 後半：Out
			return 0.5f + 0.5f * EaseValue(EaseType::OutQuart, t * 2.f);
		}
	}
	// 急減速してから急加速
	case Easing::EaseType::OutInQuart:
	{
		if (t < 0.5f) {
			// 前半：Out
			return 0.5f * EaseValue(EaseType::OutQuart, t * 2.f);
		}
		else {
			// 後半：In
			return 0.5f + 0.5f * EaseValue(EaseType::InQuart, t * 2.f - 1.f);
		}
	}
	// ゴールを少し通り過ぎてから戻る
	case Easing::EaseType::OutBack:
	{
		float n = BackOvershoot;
		float m = n + 1;
		return 1.f + m * std::pow(t - 1.f, 3.f) + n * std::pow(t - 1.f, 2.f);
	}
	// ゴールでばねのように揺れる
	case Easing::EaseType::OutElastic:
	{
		if (t == 0.f)	   return 0.f;
		else if (t == 1.f) return 1.f;
		else
		{
			float c = (ElasticAmplitudeBase * DirectX::XM_PI) / ElasticPeriodDivisor;
			return std::pow(ElasticAmplitudeBase, -ElasticDecayRate * t) *
				std::sin((t * ElasticOscillationCount - ElasticPhaseShift) * c) + 1;
		}
	}
	// ゴールで跳ね返る
	case Easing::EaseType::OutBounce:
	{
		float a = BounceCoefficient;	// 係数
		float k = BounceIntervalScale;	// 区間の基準値

		if (t < 1.f / k)		 return a * t * t;
		else if (t < 2.f / k)	 return a * (t -= BounceSecondStart / k) * t + BounceSecondHeight;
		else if (t < 2.5f / k)  return a * (t -= BounceThirdStart / k) * t + BounceThirdHeight;
		else					 return a * (t -= BounceFourthStart / k) * t + BounceFourthHeight;
	}
	}
	return t;
}