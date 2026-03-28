/**
 * @file   Object.cpp
 *
 * @brief  オブジェクトに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Object.h"
#include "Source/Game/Message/Message.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
Object::Object()
	:
	m_currentState{},
	m_currentMessage{}
{
}



/**
 * @brief デストラクタ
 */
Object::~Object()
{
}



/**
 * @brief 初期化処理
 *
 * @param なし
 *
 * @return なし
 */
void Object::Initialize()
{ 
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void Object::Update(const float& elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 * @param debugFont デバッグフォント
 *
 * @return なし
 */
void Object::Draw(const RenderContext& context, DebugFont* debugFont)
{
	UNREFERENCED_PARAMETER(context);
	UNREFERENCED_PARAMETER(debugFont);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Object::Finalize()
{ 
}
