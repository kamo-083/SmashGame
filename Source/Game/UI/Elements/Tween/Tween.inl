/**
 * @file   Tween.inl
 *
 * @brief  トゥイーンに関するインラインファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Tween.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param data	トゥイーン情報
 */
template<typename TVec, typename TRot>
Tween<TVec,TRot>::Tween(typename Tween<TVec, TRot>::TweenData data)
	: m_data{ data }
	, m_elapsedTime{ 0.0f }
	, m_playing{ false }
	, m_finished{ false }
	, m_played{ false }
	, m_reverse{ false }
{
	if (m_data.loop == PlaybackMode::Once_Reverse)
	{
		m_reverse = true;
	}
}


/**
 * @brief デストラクタ
 */
template<typename TVec, typename TRot>
Tween<TVec, TRot>::~Tween()
{

}


/**
 * @brief 更新処理
 *
 * @param deltaTime	経過時間
 * @param params	変化させるパラメータ
 *
 * @return なし
 */
template<typename TVec, typename TRot>
void Tween<TVec, TRot>::Update(float deltaTime, UIParams& params)
{
	if (!m_playing || m_finished) return;

	m_elapsedTime += deltaTime;

	float t = m_elapsedTime / m_data.duration;
	if (m_reverse) t = 1.0f - t;

	float n = EaseValue(m_data.ease, t);

	// 位置を補間
	params.pos = m_data.start.pos + m_data.delta.pos * n;

	// 大きさを補間
	params.scale = m_data.start.scale + m_data.delta.scale * n;

	// 回転を補間
	//float endAngle = m_data.start.rotation + m_data.delta.rotation;
	//float deltaAngle = ShortestAngle(endAngle - m_data.start.rotation);
	//params.rotation = m_data.start.rotation + deltaAngle * n;
	
	// 不透明度を補間
	params.opacity = m_data.start.opacity + m_data.delta.opacity * n;

	if (m_elapsedTime >= m_data.duration)
	{
		switch (m_data.loop)
		{
		case PlaybackMode::Once:
		case PlaybackMode::Once_Reverse:
			m_finished = true;
			m_playing = false;
			break;
		case PlaybackMode::Repeat:
			m_elapsedTime = 0.0f;
			break;
		case PlaybackMode::PingPong:
			m_reverse = !m_reverse;
			m_elapsedTime = 0.0f;
			break;
		}
	}
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
template<typename TVec, typename TRot>
void Tween<TVec, TRot>::Finalize()
{

}



/**
 * @brief 再生
 *
 * @param なし
 *
 * @return なし
 */
template<typename TVec, typename TRot>
void Tween<TVec, TRot>::Play()
{
	m_playing = true;
	m_finished = false;
	m_played = true;
}



/**
 * @brief 停止
 *
 * @param なし
 *
 * @return なし
 */
template<typename TVec, typename TRot>
void Tween<TVec, TRot>::Stop()
{
	m_playing = false;
}



/**
 * @brief 再生の終了
 *
 * @param なし
 *
 * @return なし
 */
template<typename TVec, typename TRot>
inline void Tween<TVec, TRot>::End()
{
	m_playing = false;
	m_finished = true;
	m_playing = false;
}



/**
 * @brief 再生時間のリセット
 *
 * @param なし
 *
 * @return なし
 */
template<typename TVec, typename TRot>
void Tween<TVec, TRot>::ResetTime()
{
	m_elapsedTime = 0.0f;

	if (m_data.loop == PlaybackMode::Once_Reverse)	m_reverse = true;
	else											m_reverse = false;
}