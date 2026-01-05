/**
 * @file   KeyAtlas.h
 *
 * @brief  キーアイコンの切り取りに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================



// クラスの定義 ===============================================================
/**
 * @brief キーアイコンの切り取り
 */
class KeyAtlas
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static const std::vector<std::string> KEY_INDEX;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	KeyAtlas();

	// デストラクタ
	~KeyAtlas() = default;


// 操作・取得
public:
	// 切り取り範囲を取得
	static RECT GetRect(DirectX::Keyboard::Keys key, long size);

// 内部実装
private:
	// 文字列からキー番号を取得
	static int GetIndex(const std::string& name);

	// Keysからキー番号を取得
	static int GetIndex(const DirectX::Keyboard::Keys& key);

};
