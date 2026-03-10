/**
 * @file   Messenger.cpp
 *
 * @brief  メッセンジャーに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Messenger.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
Messenger::Messenger()
{

}


/**
 * @brief オブジェクトを登録
 *
 * @param listenerID ID
 * @param listener	 オブジェクトのポインタ
 *
 * @return なし
 */
void Messenger::AddObject(int listenerID, IMessageListener* listener)
{
	// オブジェクトを配列に追加
	m_listeners[listenerID] = listener;
}


/**
 * @brief メッセージを送信
 *
 * @param listenerID 送信先のID
 * @param messageID  メッセージID
 *
 * @return なし
 */
void Messenger::Notify(int listenerID, Message::MessageID messageID)
{
	// 送信先を検索
	auto it = m_listeners.find(listenerID);
	if (it == m_listeners.end() || it->second == nullptr) return;

	// イベントを送信
	it->second->OnMessageAccepted(messageID);
	
}

