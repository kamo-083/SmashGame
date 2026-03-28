/**
 * @file	Object.h
 *
 * @brief  オブジェクトに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

 // ヘッダファイルの読み込み ===================================================
#include "Source/Game/Interface/IMessageListener.h"
#include "Source/Game/Interface/IState.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Debug/DebugFont.h"

// クラスの定義 ===============================================================
/**
 * @brief オブジェクト
 */
class Object : public IMessageListener
{
// クラス定数の宣言 -------------------------------------------------
public:

// データメンバの宣言 -----------------------------------------------
private:
	// 現在の状態
	IState* m_currentState;

	// 現在のメッセージ
	Message m_currentMessage;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Object();
	 
	// デストラクタ
	virtual ~Object();

// 操作
public:
	// 初期化処理
	virtual void Initialize();

	// 更新処理
	virtual void Update(const float& elapsedTime);

	// 描画処理
	virtual void Draw(const RenderContext& context, DebugFont* debugFont);

	// 終了処理
	virtual void Finalize();

// 取得/設定
public:
	// 状態を取得・設定
	IState* GetState() { return m_currentState; }
	void SetState(IState* state) { m_currentState = state; }

// 内部実装
private:

};