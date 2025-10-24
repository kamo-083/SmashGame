/**
 * @file   TweenBase.cpp
 *
 * @brief  トゥイーンのベースに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TweenBase.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief イージングの計算
 *
 * @param ease	イージングの種類
 * @param t		アニメーションの進行度
 *
 * @return なし
 */
float TweenBase::EaseValue(Ease ease, float t)
{

	switch (ease)
	{
	case TweenBase::Ease::Liner:
	{
		return t;
	}
	case TweenBase::Ease::InQuart:
	{
		return std::pow(t, 4.f);
	}
	case TweenBase::Ease::OutQuart:
	{
		return 1.f - std::pow(1.f - t, 4.f);
	}
	case TweenBase::Ease::OutInQuart:
	{
		if (t < 0.5f) {
			// 前半：Out
			return 0.5f * EaseValue(Ease::OutQuart, t * 2.f);
		}
		else {
			// 後半：In
			return 0.5f + 0.5f * EaseValue(Ease::InQuart, t * 2.f - 1.f);
		}
	}
	case TweenBase::Ease::OutBack:
	{
		float n = 1.70158f;
		float m = n + 1;
		return 1.f + m * std::pow(t - 1.f, 3.f) + n * std::pow(t - 1.f, 2.f);
	}
	case TweenBase::Ease::OutElastic:
	{
		if (t == 0.f)	   return 0.f;
		else if (t == 1.f) return 1.f;
		else 
		{
			float c = (2.f * DirectX::XM_PI) / 3.f;
			return std::pow(2.f, -10.f * t) * std::sin((t * 10.f - 0.75f) * c) + 1;
		}
	}
	case TweenBase::Ease::OutBounce:
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



/**
 * @brief 最短回転角度の計算
 *
 * @param delta	回転角度
 *
 * @return なし
 */
float TweenBase::ShortestAngle(float delta)
{
	if (delta > DirectX::XM_PI)  delta -= DirectX::XM_2PI;
	if (delta < -DirectX::XM_PI) delta += DirectX::XM_2PI;
	return delta;
}

