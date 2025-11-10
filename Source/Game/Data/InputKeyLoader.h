/**
 * @file   InputKeyLoader.h
 *
 * @brief  入力キー設定ファイル用ローダーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include"Source/Game/Common/KeyConverter.h"

using json = nlohmann::json;


// クラスの定義 ===============================================================
/**
 * @brief 入力キー設定ファイル用ローダー
 */
class InputKeyLoader
{
	// クラス定数の宣言 -------------------------------------------------
public:
	struct InputKeyInfo
	{
		DirectX::Keyboard::Keys move_forward;
		DirectX::Keyboard::Keys move_backward;
		DirectX::Keyboard::Keys move_left;
		DirectX::Keyboard::Keys move_right;
		DirectX::Keyboard::Keys attack;
	};


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	InputKeyLoader()
	{};

	// デストラクタ
	~InputKeyLoader() = default;


public:
	/**
	 * @brief ファイル読み込み
	 *
	 * @param path		 ファイルパス
	 * @param outputInfo 情報出力用
	 *
	 * @return 読み込みに成功/失敗
	 */
	static bool LoadData(const std::string& path, InputKeyInfo& outputInfo)
	{
		// ファイルを開く
		std::ifstream stream(path);

		// ファイルを開けたかチェック
		if (!stream.is_open()) return false;

		// jsonのフォーマットが不正でないかチェック
		if (!json::accept(stream)) return false;

		// 先頭に戻す
		stream.clear();
		stream.seekg(0, std::ios::beg);

		json j = json::parse(stream);

		// データを読み込む
		InputKeyInfo data;

		if (j.contains("move_forward") && j["move_forward"].is_string())
		{
			data.move_forward = KeyConverter::ConvertToKeys(j["move_forward"]);
		}

		if (j.contains("move_backward") && j["move_backward"].is_string())
		{
			data.move_backward = KeyConverter::ConvertToKeys(j["move_backward"]);
		}
		
		if (j.contains("move_left") && j["move_left"].is_string())
		{
			data.move_left = KeyConverter::ConvertToKeys(j["move_left"]);
		}
		
		if (j.contains("move_right") && j["move_right"].is_string())
		{
			data.move_right = KeyConverter::ConvertToKeys(j["move_right"]);
		}
		
		if (j.contains("attack") && j["attack"].is_string())
		{
			data.attack = KeyConverter::ConvertToKeys(j["attack"]);
		}

		// 情報を登録
		outputInfo = data;

		return true;
	}
};
