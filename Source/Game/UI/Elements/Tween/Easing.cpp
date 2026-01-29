/**
 * @file   Easing.cpp
 *
 * @brief  イージングの計算に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Easing.h"


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
	case Easing::EaseType::Linear:
	{
		return t;
	}
	case Easing::EaseType::InSine:
	{
		return 1 - std::cos((t * DirectX::XM_PI) / 2);
	}
	case Easing::EaseType::OutSine:
	{
		return std::cos((t * DirectX::XM_PI) / 2);
	}
	case Easing::EaseType::InOutSine:
	{
		return -(std::cos(DirectX::XM_PI * t) - 1) / 2;
	}
	case Easing::EaseType::InQuart:
	{
		return std::pow(t, 4.f);
	}
	case Easing::EaseType::OutQuart:
	{
		return 1.f - std::pow(1.f - t, 4.f);
	}
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
	case Easing::EaseType::OutBack:
	{
		float n = 1.70158f;
		float m = n + 1;
		return 1.f + m * std::pow(t - 1.f, 3.f) + n * std::pow(t - 1.f, 2.f);
	}
	case Easing::EaseType::OutElastic:
	{
		if (t == 0.f)	   return 0.f;
		else if (t == 1.f) return 1.f;
		else
		{
			float c = (2.f * DirectX::XM_PI) / 3.f;
			return std::pow(2.f, -10.f * t) * std::sin((t * 10.f - 0.75f) * c) + 1;
		}
	}
	case Easing::EaseType::OutBounce:
	{
		float a = 7.5625;	// 係数
		float k = 2.75f;	// 区間の基準値

		if (t < 1.f / k)		 return a * t * t;
		else if (t < 2.f / k)	 return a * (t -= 1.5f / k) * t + 0.75f;
		else if (t < 2.5f / k)  return a * (t -= 2.25f / k) * t + 0.9375f;
		else					 return a * (t -= 2.625f / k) * t + 0.984375f;
	}
	}
	return t;
}