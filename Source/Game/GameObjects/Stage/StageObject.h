/**
 * @file	StageObject.h
 *
 * @brief  ステージオブジェクトの基底クラスに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

 // ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Data/StageEventData.h"
#include "Source/Game/Interface/IStageEventDispatcher.h"
#include "Source/Debug/DebugFont.h"

// クラスの定義 ===============================================================
/**
 * @brief ステージオブジェクトの基底クラス
 */
class StageObject
{
// クラス定数の宣言 -------------------------------------------------
public:
	// 作成時の初期データ群
	struct StageObjectDesc
	{
		const std::string& stageObject_ID;			// オブジェクトID
		const std::vector<EventHandle>& events;		// イベント群
		IStageEventDispatcher* pEventDispatcher;	// イベント受け渡し口のポインタ
	}; 

	struct StageEventContext
	{
		StageEvent event;		// イベントの種類
		std::string targetID;	// 対象ID
		std::string senderID;	// 送り主ID
	};

	// ステージオブジェクトのID
	const std::string STAGE_OBJECT_ID;

// データメンバの宣言 -----------------------------------------------
protected:
	// イベント
	std::vector<EventHandle> m_events;

	// ステージイベント受け渡し口へのポインタ
	IStageEventDispatcher* m_pEventDispatcher;

	// 位置
	DirectX::SimpleMath::Vector3 m_position;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageObject(const StageObjectDesc& desc);

	// デストラクタ
	virtual ~StageObject();

// 操作
public:
	// 初期化処理
	virtual void Initialize();

	// 更新処理
	virtual void Update(float elapsedTime);

	// 描画処理
	virtual void Draw(const RenderContext& context, DebugFont* debugFont) = 0;

	// 終了処理
	virtual void Finalize();

	// イベントの受け取り
	virtual void OnStageEvent(StageEventContext context) = 0;

	// イベント発火
	void FireEvent();

// 取得/設定
public:
	// ステージオブジェクトIDの取得
	std::string GetStageObjectID() { return STAGE_OBJECT_ID; }

	// イベントの取得
	const std::vector<EventHandle>& GetEvents() { return m_events; }

	// 位置の取得
	const DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
};