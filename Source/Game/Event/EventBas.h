/**
 * @file   EventBas.h
 *
 * @brief  イベントの送受信に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Event/Event.h"
#include"Source/Game/Interface/IEventHandler.h"


// クラスの定義 ===============================================================
/**
 * @brief イベントの送受信
 */
class EventBas
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:
	// オブジェクトのポインタ群
	std::vector<IEventHandler*> m_handlers;
		

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EventBas();

	// デストラクタ
	~EventBas() = default;


// 操作
public:
	// オブジェクトを登録
	void AddHandler(IEventHandler* handler);

	// イベントを送信
	void Notify( Event::EventID eventID);


// 取得/設定
public:

// 内部実装
private:

};
