/**
 * @file   AudioListener.cpp
 *
 * @brief  音声再生用のメッセージ受信に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "AudioListener.h"
#include "Source/Game/Message/Message.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
AudioListener::AudioListener(AudioManager* pAM)
	: m_pAudio{ pAM }
{
}



/**
 * @brief デストラクタ
 */
AudioListener::~AudioListener()
{
}



/**
 * @brief 初期化処理
 *
 * @param なし
 *
 * @return なし
 */
void AudioListener::Initialize()
{
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void AudioListener::Finalize()
{ 
}



/**
 * @brief メッセージの受信
 *
 * @param messageID メッセージID
 *
 * @return なし
 */
void AudioListener::OnMessageAccepted(Message::MessageID messageID)
{
	// SEを再生
	switch (messageID)
	{
	case Message::MessageID::SE_BOUNCE:			// 跳ね返る
		m_pAudio->Play("bounceSE", false);
		break;
	case Message::MessageID::SE_ATTACK:			// 攻撃
		m_pAudio->Play("attackSE", false);
		break;
	case Message::MessageID::SE_CHANGE_ATTACK:	// 攻撃の種類を変更
		m_pAudio->Play("cursorSE", false);
		break;
	case Message::MessageID::SE_CURSOR:			// カーソル移動
		m_pAudio->Play("cursorSE", false);
		break;
	case Message::MessageID::SE_GIMMIC_SOLVE:	// ギミックを解いた
		m_pAudio->Play("gimmicSolveSE", false);
		break;
	case Message::MessageID::SE_KEY_SPAWN:		// カギの出現
		m_pAudio->Play("keySpawnSE", false);
		break;
	case Message::MessageID::SE_GOAL_OPEN:		// ゴールの開放
		m_pAudio->Play("canGoalSE", false);
		break;
	case Message::MessageID::SE_STAGE_CLEAR:	// ステージクリア
		m_pAudio->Play("clearSE", false);
		break;
	}
}
