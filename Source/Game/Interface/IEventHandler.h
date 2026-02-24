/**
 * @file   IEventHandler.h
 *
 * @brief  イベント利用者のインターフェースのヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み
#include"Source/Game/Event/Event.h"


class IEventHandler
{
public:
	// デストラクタ
	virtual ~IEventHandler() = default;

	// イベントの受信
	virtual void OnEventAccepted(Event::EventID eventID) = 0;
};