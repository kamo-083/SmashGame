/**
 * @file   KeyConverter.cpp
 *
 * @brief  文字列->Keysの変換に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "KeyConverter.h"


// クラス定数の定義 ===========================================================
const std::unordered_map<std::string, DirectX::Keyboard::Keys> KeyConverter::KEY_MAP =
{
	{"A", DirectX::Keyboard::Keys::A},
	{"B", DirectX::Keyboard::Keys::B},
	{"C", DirectX::Keyboard::Keys::C},
	{"D", DirectX::Keyboard::Keys::D},
	{"E", DirectX::Keyboard::Keys::E},
	{"F", DirectX::Keyboard::Keys::F},
	{"G", DirectX::Keyboard::Keys::G},
	{"H", DirectX::Keyboard::Keys::H},
	{"I", DirectX::Keyboard::Keys::I},
	{"J", DirectX::Keyboard::Keys::J},
	{"K", DirectX::Keyboard::Keys::K},
	{"L", DirectX::Keyboard::Keys::L},
	{"M", DirectX::Keyboard::Keys::M},
	{"N", DirectX::Keyboard::Keys::N},
	{"O", DirectX::Keyboard::Keys::O},
	{"P", DirectX::Keyboard::Keys::P},
	{"Q", DirectX::Keyboard::Keys::Q},
	{"R", DirectX::Keyboard::Keys::R},
	{"S", DirectX::Keyboard::Keys::S},
	{"T", DirectX::Keyboard::Keys::T},
	{"U", DirectX::Keyboard::Keys::U},
	{"V", DirectX::Keyboard::Keys::V},
	{"W", DirectX::Keyboard::Keys::W},
	{"X", DirectX::Keyboard::Keys::X},
	{"Y", DirectX::Keyboard::Keys::Y},
	{"Z", DirectX::Keyboard::Keys::Z},
	{"Up", DirectX::Keyboard::Keys::Up},
	{"Down", DirectX::Keyboard::Keys::Down},
	{"Left", DirectX::Keyboard::Keys::Left},
	{"Right", DirectX::Keyboard::Keys::Right},
	{"Space", DirectX::Keyboard::Keys::Space},
};

const std::vector<std::string> KeyConverter::KEY_INDEX =
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
KeyConverter::KeyConverter()
{

}



/**
 * @brief  文字列->Keysの変換
 *
 * @param name キー名
 *
 * @return 対応したキー
 */
DirectX::Keyboard::Keys KeyConverter::ConvertToKeys(const std::string& name)
{
	// キーを探して返す
	auto it = KEY_MAP.find(name);
	if (it != KEY_MAP.end()) return it->second;

	// 無かった場合
	return DirectX::Keyboard::Keys::None;
}



/**
 * @brief  Keys->文字列の変換
 *
 * @param key キー
 *
 * @return キー名
 */
std::string KeyConverter::ConvertToString(const DirectX::Keyboard::Keys& key)
{
	for (auto it = KEY_MAP.begin(); it != KEY_MAP.end(); ++it)
	{
		const std::string& name = it->first;
		DirectX::Keyboard::Keys mappedKey = it->second;

		// 一致していたら返す
		if (mappedKey == key) return name;
	}

	// 無かった場合
	return "None";
}



/**
 * @brief  文字列からキー番号を取得
 *
 * @param name キー名
 *
 * @return キー番号
 */
int KeyConverter::GetIndex(const std::string& name)
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
int KeyConverter::GetIndex(const DirectX::Keyboard::Keys& key)
{
	// Keys->文字列の変換
	const std::string& name = ConvertToString(key);
	
	// キー番号を取得
	return GetIndex(name);
}