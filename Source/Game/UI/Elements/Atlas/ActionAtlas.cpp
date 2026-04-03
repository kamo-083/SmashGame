/**
 * @file   ActionAtlas.cpp
 *
 * @brief  動作テキスト画像の切り取りに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ActionAtlas.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
ActionAtlas::ActionAtlas()
{
}

/**
 * @brief 切り取り範囲を取得
 *
 * @param type 動作の種類
 * @param size １文字の大きさ(縦横)
 *
 * @return 切り取り範囲
 */
RECT ActionAtlas::GetRect(ActionType type, long size)
{
	// 種類をintに変換
	int t = static_cast<int>(type);

	// 切り取り範囲を計算
	RECT rect;
	rect.left = 0;
	rect.right = size * TEXT_LENGTH_TABLE[t];
	rect.top = size * t;
	rect.bottom = rect.top + size;

	return rect;
}