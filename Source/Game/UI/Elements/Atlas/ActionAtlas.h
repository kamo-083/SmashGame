/**
 * @file   ActionAtlas.h
 *
 * @brief  動作テキスト画像の切り取りに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================



// クラスの定義 ===============================================================
/**
 * @brief 動作テキスト画像の切り取り
 */
class ActionAtlas
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 動作の種類
	enum class ActionType
	{
		SEPARATOR,	// 区切り記号
		SELECT,		// 選択
		DECIDE,		// 決定
		TO_TITLE,	// タイトルへ
		OPEN_PAUSE,	// ポーズ

		TypeNum		// 種類数
	};

	// 種類ごとの文字数
	static constexpr int TEXT_LENGTH_TABLE[static_cast<int>(ActionType::TypeNum)] =
	{
		1,	// 区切り記号
		2,	// 選択
		2,	// 決定
		5, 	// タイトルへ
		3	// ポーズ
	};


	// データメンバの宣言 -----------------------------------------------
private:



	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ActionAtlas();

	// デストラクタ
	~ActionAtlas() = default;


// 操作・取得
public:
	// 切り取り範囲を返す
	static RECT GetRect(ActionType type, long size);


// 内部実装
private:

};
