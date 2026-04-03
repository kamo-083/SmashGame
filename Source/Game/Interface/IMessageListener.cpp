#include "pch.h"
#include "IMessageListener.h"

// オブジェクト番号
int IMessageListener::s_listenerID = -1;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
IMessageListener::IMessageListener()
	:m_listenerID{ CountUpID() }
{
}