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
		m_playing = false;
		m_finished = true;
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


float Tween::EaseValue(Ease ease, float t)
{
	switch (ease)
	{
	case Tween::Ease::OutQuart:
	{
		float n = 1.0f - t;
		return 1 - (n * n * n * n);
	}
	case Tween::Ease::OutBounce:
	{
		float n1 = 7.5625;
		float d1 = 2.75;

		if (t < 1.f / d1)		 return n1 * t * t;
		else if (t < 2.f / d1)	 return n1 * (t -= 1.5f / d1) * t + 0.75f;
		else if (t < 2.25f / d1) return n1 * (t -= 2.25f / d1) * t + 0.9375f;
		else					 return n1 * (t -= 2.625f / d1) * t + 0.984375f;
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

