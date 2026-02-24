/**
 * @file   EventBas.cpp
 *
 * @brief  イベントの送受信に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EventBas.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
EventBas::EventBas()
{

}


/**
 * @brief オブジェクトを登録
 *
 * @param handler オブジェクトのポインタ
 * 
 * @return なし
 */
void EventBas::AddHandler(IEventHandler* handler)
{
	// オブジェクトを配列に追加
	m_handlers.emplace_back(handler);
}


/**
 * @brief イベントを送信
 *
 * @param eventID イベントID
 *
 * @return なし
 */
void EventBas::Notify(Event::EventID eventID)
{
	// 各オブジェクトにイベントを送信
	for (IEventHandler* handler : m_handlers)
	{
		handler->OnEventAccepted(eventID);
	}
}

