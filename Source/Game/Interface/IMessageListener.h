/**
 * @file   IMessageListener.h
 *
 * @brief  メッセージ利用者のインターフェースのヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


 // ヘッダファイルの読み込み ===================================================
#include"Source/Game/Message/Message.h"


// クラスの定義 ===============================================================
/**
 * @brief メッセージ利用者のインターフェース
 */
class IMessageListener
{
	// メンバ関数の宣言 -------------------------------------------------
public:
	// デストラクタ
	virtual ~IMessageListener() = default;

	// イベントの受信
	virtual void OnMessageAccepted(Message::MessageID messageID) = 0;
};
