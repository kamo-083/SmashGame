/**
 * @file   Tween.cpp
 *
 * @brief  Tweenに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Tween.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Tween::Tween(TweenData data)
	: m_data{ data }
	, m_elapsedTime{ 0.0f }
	, m_playing{ false }
	, m_finished{ false }
{

}


/**
 * @brief デストラクタ
 */
Tween::~Tween()
{

}


/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Tween::Update(float deltaTime, UIParams& params)
{
	if (!m_playing || m_finished) return;

	m_elapsedTime += deltaTime;

	float n = EaseValue(m_data.ease, m_elapsedTime / m_data.duration);

	// 位置を補間
	params.pos = m_data.start.pos + m_data.delta.pos * n;

	// 大きさを補間
	params.scale = m_data.start.scale + m_data.delta.scale * n;

	// 回転を補間
	float endAngle = m_data.start.rotation + m_data.delta.rotation;
	float deltaAngle = ShortestAngle(endAngle - m_data.start.rotation);
	params.rotation = m_data.start.rotation + deltaAngle * n;
	
	// 不透明度を補間
	params.opacity = m_data.start.opacity + m_data.delta.opacity * n;

	if (m_elapsedTime >= m_data.duration)
	{
		if (m_data.loop)	// ループだったら
		{
			m_elapsedTime = 0.0f;
		}
		else
		{
			m_playing = false;
			m_finished = true;
		}
	}
}


/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Tween::Finalize()
{

}


void Tween::Play()
{
	m_playing = true;
}


void Tween::Stop()
{
	m_playing = false;
}

void Tween::ResetTime()
{
	m_elapsedTime = 0.0f;
}


float Tween::EaseValue(Ease ease, float t)
{
	switch (ease)
	{
	case Tween::Ease::Liner:
	{
		return t;
	}
	case Tween::Ease::OutQuart:
	{
		return 1.f - std::pow(1.f - t, 4.f);
	}
	case Tween::Ease::OutBack:
	{
		float n = 1.70158f;
		float m = n + 1;
		return 1.f + m * std::pow(t - 1.f, 3.f) + n * std::pow(t - 1.f, 2.f);
	}
	case Tween::Ease::OutElastic:
	{
		if (t == 0.f)	   return 0.f;
		else if (t == 1.f) return 1.f;
		else 
		{
			float c = (2.f * XM_PI) / 3.f;
			return std::pow(2.f, -10.f * t) * std::sin((t * 10.f - 0.75f) * c) + 1;
		}
	}
	case Tween::Ease::OutBounce:
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


float Tween::ShortestAngle(float delta)
{
	if (delta > XM_PI)  delta -= XM_2PI;
	if (delta < -XM_PI) delta += XM_2PI;
	return delta;
}

