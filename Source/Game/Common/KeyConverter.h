/**
 * @file   KeyConverter.h
 *
 * @brief  文字列->Keysの変換に関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
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

	static const std::vector<std::string> KEY_INDEX;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	KeyConverter();

	// デストラクタ
	~KeyConverter() = default;


// 操作
public:
	// 文字列->Keysの変換
	static DirectX::Keyboard::Keys ConvertToKeys(const std::string& name);
	
	// 文字列からキー番号を取得
	static int GetIndex(const std::string& name);
};
