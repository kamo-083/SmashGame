/**
 * @file   EnemyInfoLoader.h
 *
 * @brief  敵情報ファイル用ローダーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include "Source/Game/Data/EnemyData.h"

using json = nlohmann::json;


// クラスの定義 ===============================================================
/**
 * @brief 敵情報ファイル用ローダー
 */
class EnemyInfoLoader
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// アニメーションの種類
	enum class AnimType
	{
		Idle,	// 待機
		Walk,	// 移動
		Attack,	// 攻撃
	};

	// 攻撃の情報
	struct AttackDesc
	{
		float time = 0.0f;		// 攻撃時間
		float size = 0.0f;		// 判定サイズ
		float force = 0.0f;		// 攻撃力
		float distance_raito = 1.0f;	// 攻撃判定の距離倍率
	};

	// 敵の情報
	struct EnemyInfo
	{
		EnemyType type = EnemyType::Basic;	// 種類
		float radius = 0.0f;				// 半径サイズ
		float mass = 0.0f;					// 質量
		float move_speed = 0.0f;			// 移動速度
		float max_speed = 0.0f;				// 最高移動速度
		float static_friction = 0.0f;		// 静止摩擦係数
		float dynamic_friction = 0.0f;		// 動摩擦係数
		float restitution = 0.0f;			// 反発係数
		AttackDesc attack{};				// 攻撃情報

		std::string modelPath;		// モデルのファイルパス
		std::string idleAnimPath;	// 待機アニメーションのファイルパス
		std::string walkAnimPath;	// 移動アニメーションのファイルパス
		std::string attackAnimPath;	// 攻撃アニメーションのファイルパス
	};


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EnemyInfoLoader()
	{}

	// デストラクタ
	~EnemyInfoLoader() = default;


public:
	/**
	 * @brief ファイル読み込み
	 *
	 * @param path		 ファイルパス
	 * @param outputInfo 情報出力用
	 *
	 * @return 読み込みに成功/失敗
	 */
	bool LoadData(const std::string& path, std::vector<EnemyInfo>& outputInfo)
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
		outputInfo.clear();
		for (auto& element : j["info"])
		{
			EnemyInfo data;

			// 種類
			std::string typeStr = element.value("type", "Basic");
			if (typeStr == "Basic") data.type = EnemyType::Basic;
			else if (typeStr == "Heavy") data.type = EnemyType::Heavy;
			else if (typeStr == "Light") data.type = EnemyType::Light;

			// 大きさ
			if (element.contains("radius") && element["radius"].is_number())
			{
				data.radius = element["radius"];
			}

			// 質量
			if (element.contains("mass") && element["mass"].is_number())
			{
				data.mass = element["mass"];
			}

			// 移動速度
			if (element.contains("move_speed") && element["move_speed"].is_number())
			{
				data.move_speed = element["move_speed"];
			}

			// 最高速度
			if (element.contains("max_speed") && element["max_speed"].is_number())
			{
				data.max_speed = element["max_speed"];
			}

			// 静止摩擦係数
			if (element.contains("static_friction") && element["static_friction"].is_number())
			{
				data.static_friction = element["static_friction"];
			}

			// 動摩擦係数
			if (element.contains("dynamic_friction") && element["dynamic_friction"].is_number())
			{
				data.dynamic_friction = element["dynamic_friction"];
			}
			
			// 反発係数
			if (element.contains("restitution") && element["restitution"].is_number())
			{
				data.restitution = element["restitution"];
			}

			// 攻撃
			if (element.contains("attack") && element["attack"].is_array())
			{
				data.attack = AttackDesc
				{
					element["attack"][0].get<float>(),
					element["attack"][1].get<float>(),
					element["attack"][2].get<float>(),
					element["attack"][3].get<float>()
				};
			}

			// モデルのパス
			if (element.contains("modelPath") && element["modelPath"].is_string())
			{
				data.modelPath = element["modelPath"];
			}

			// 待機アニメーションのパス
			if (element.contains("idleAnimPath") && element["idleAnimPath"].is_string())
			{
				data.idleAnimPath = element["idleAnimPath"];
			}

			// 移動アニメーションのパス
			if (element.contains("walkAnimPath") && element["walkAnimPath"].is_string())
			{
				data.walkAnimPath = element["walkAnimPath"];
			}

			// 攻撃アニメーションのパス
			if (element.contains("attackAnimPath") && element["attackAnimPath"].is_string())
			{
				data.attackAnimPath = element["attackAnimPath"];
			}

			// 配列に追加
			outputInfo.push_back(std::move(data));
		}

		return true;
	}
};
