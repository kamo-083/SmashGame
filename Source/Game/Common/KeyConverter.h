/**
 * @file   KeyConverter.h
 *
 * @brief  文字列->Keysの変換に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include"Keyboard.h"
#include<string>
#include<unordered_map>
#include<vector>

// クラスの定義 ===============================================================
/**
 * @brief 文字列->Keysの変換
 */
class KeyConverter
{
// クラス定数の宣言 -------------------------------------------------
private:
	static const std::unordered_map<std::string, DirectX::Keyboard::Keys> KEY_MAP;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	KeyConverter();

	// デストラクタ
	~KeyConverter() = default;

// 操作・取得
public:
	// 文字列->Keysの変換
	static DirectX::Keyboard::Keys ConvertToKeys(const std::string& name);
	
	// Keys->文字列の変換
	static std::string ConvertToString(const DirectX::Keyboard::Keys& key);
};