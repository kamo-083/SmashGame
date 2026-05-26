/**
 * @file   IStageEventDispatcher.h
 *
 * @brief  ステージイベント関連の受け渡しインターフェースに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Data/StageEventData.h"

// クラスの定義 ===============================================================
/**
 * @brief IStageEventDispatcher
 */
class IStageEventDispatcher
{
// メンバ関数の宣言 -------------------------------------------------
// デストラクタ
public:
	virtual ~IStageEventDispatcher() = default;

// 操作
public:
	virtual void DispatchEvent(const EventHandle& handle, const std::string& senderID) = 0;

	virtual DirectX::SimpleMath::Vector3 DispatchPosition(const std::string& objectID) = 0;
};
