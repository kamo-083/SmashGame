/**
 * @file   KeyAtlas.cpp
 *
 * @brief  キーテキスト画像の切り取りに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "KeyAtlas.h"
#include "Source/Game/Common/KeyConverter.h"

// クラス定数の定義 ===========================================================
const std::vector<std::string> KeyAtlas::KEY_INDEX =
{
	"A", "B", "C", "D",
	"E", "F", "G", "H",
	"I", "J", "K", "L",
	"M", "N", "O", "P",
	"Q", "R", "S", "T",
	"U", "V", "W", "X",
	"Y", "Z",
	"Up", "Down", "Left", "Right",
	"Space",
};

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
KeyAtlas::KeyAtlas()
{
}

/**
 * @brief  切り取り範囲を取得
 *
 * @param key	キー
 * @param size	1文字の大きさ(縦横)
 *
 * @return 切り取り範囲
 */
RECT KeyAtlas::GetRect(DirectX::Keyboard::Keys key, long size)
{
	// 文字テクスチャの切り取り範囲を計算
	int keyIndex = GetIndex(key);
	RECT rect;
	if (keyIndex >= 30)
	{
		// 特殊キー(4列目以降)
		long height = size * (keyIndex - 30 + 3);
		rect =
		{
			0,		  height,
			size * 3, height + size
		};
	}
	else if (keyIndex >= 26)
	{
		// 矢印キー(3列目)
		long height = size * 2;
		long width = size * (keyIndex - 26);
		rect =
		{
			width,		  height,
			width + size, height + size
		};
	}
	else if (keyIndex >= 13)
	{
		// アルファベット2列目
		long width = size * (keyIndex - 13);
		rect =
		{
			width,		  size,
			width + size, size * 2
		};
	}
	else
	{
		// アルファベット1列目
		long width = size * keyIndex;
		rect =
		{
			width,		  0,
			width + size, size
		};
	}

	return rect;
}

/**
 * @brief  文字列からキー番号を取得
 *
 * @param name キー名
 *
 * @return キー番号
 */
int KeyAtlas::GetIndex(const std::string& name)
{
	// 配列内からキーを探す
	auto it = std::find(KEY_INDEX.begin(), KEY_INDEX.end(), name);
	if (it == KEY_INDEX.end()) return -1;	// 無かった場合

	return static_cast<int>(std::distance(KEY_INDEX.begin(), it));
}

/**
 * @brief  Keysからキー番号を取得
 *
 * @param key キー
 *
 * @return キー番号
 */
int KeyAtlas::GetIndex(const DirectX::Keyboard::Keys& key)
{
	// Keys->文字列の変換
	const std::string& name = KeyConverter::ConvertToString(key);

	// キー番号を取得
	return GetIndex(name);
}