/**
 * @file   StageObject.cpp
 *
 * @brief  ステージオブジェクトの基底クラスに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageObject.h"
#include "Source/Game/Interface/IStageEventDispatcher.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param desc		ステージオブジェクトの初期データ
 */
StageObject::StageObject(const StageObjectDesc& desc)
	:
	STAGE_OBJECT_ID{desc.stageObject_ID},
	m_pEventDispatcher{desc.pEventDispatcher}
{
	m_events = desc.events;
}

/**
 * @brief デストラクタ
 */
StageObject::~StageObject()
{
}

/**
 * @brief 初期化処理
 *
 * @param なし
 *
 * @return なし
 */
void StageObject::Initialize()
{ 
}

/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void StageObject::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
}

/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void StageObject::Finalize()
{ 
}

/**
 * @brief イベント発火
 *
 * @param なし
 *
 * @return なし
 */
void StageObject::FireEvent()
{
	// 配列内のイベントを順番に渡していく
	for (EventHandle e : m_events)
	{
		m_pEventDispatcher->DispatchEvent(e, STAGE_OBJECT_ID);
	}
}
