// 多重インクルードの防止 =====================================================
#pragma once

 // ヘッダファイルの読み込み ==================================================
#include <string>

// イベントの種類
enum class StageEvent
{
	NONE = -1,
	Activate,	// 有効化
	Deactivate,	// 無効化
};

// イベントハンドル
struct EventHandle
{
	std::string objectID;	// イベントを通知するオブジェクト
	StageEvent event;		// イベントの種類
};

// 文字列からイベントに変換
inline StageEvent ToStageEvent(const std::string& eventName)
{
	if (eventName == "Activate") return StageEvent::Activate;
	if (eventName == "Deactivate") return StageEvent::Deactivate;

	// 未登録なら NONE を返す
	return StageEvent::NONE;
}