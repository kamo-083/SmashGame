/**
 * @file   Messenger.h
 *
 * @brief  メッセンジャーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Message/Message.h"
#include "Source/Game/Interface/IMessageListener.h"
#include <unordered_map>

// クラスの定義 ===============================================================
/**
 * @brief メッセンジャー
 */
class Messenger
{
// クラス定数の宣言 -------------------------------------------------
private:

// データメンバの宣言 -----------------------------------------------
private:
	// オブジェクトのポインタ群
	std::unordered_map<int, IMessageListener*> m_listeners;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Messenger();

	// デストラクタ
	~Messenger() = default;

// 操作
public:
	// オブジェクトを登録
	void AddObject(int listenerID, IMessageListener* listener);

	// イベントを送信
	void Notify(int listenerID, Message::MessageID messageID);

// 取得/設定
public:

// 内部実装
private:

};