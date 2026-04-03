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
// データメンバの宣言 -----------------------------------------------
public:
	// オブジェクト番号
	static int s_listenerID;;	// ID発行用
	int m_listenerID;			// 管理用の個別ID

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ・デストラクタ
public:
	IMessageListener();

	virtual ~IMessageListener() = default;

// 操作
public:
	// イベントの受信
	virtual void OnMessageAccepted(Message::MessageID messageID) = 0;

	// IDの取得
	int GetListenerID() const { return m_listenerID; }

// 内部実装
private:
	// オブジェクトをカウントアップする
	static int CountUpID() { return s_listenerID++; };
};