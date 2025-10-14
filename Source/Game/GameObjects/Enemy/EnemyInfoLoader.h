/**
 * @file   EnemyInfoLoader.h
 *
 * @brief  EnemyInfoLoaderに関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <sstream>

using json = nlohmann::json;


// クラスの定義 ===============================================================
/**
 * @brief EnemyInfoLoader
 */
class EnemyInfoLoader
{
	// クラス定数の宣言 -------------------------------------------------
public:
	enum class EnemyType
	{
		Basic,
		Heavy,
		Light,
	};

	enum class AnimType
	{
		Idle,
		Walk,
		Attack,
	};

	struct AttackDesc
	{
		float time;
		float size;
		float force;
	};

	struct EnemyInfo
	{
		EnemyType type;
		float radius = 0.0f;
		float mass = 0.0f;
		float speed = 0.0f;
		float max_speed = 0.0f;
		float static_friction = 0.0f;
		float dynamic_friction = 0.0f;
		AttackDesc attack;

		std::string modelPath;
		std::string idleAnimPath;
		std::string walkAnimPath;
		std::string attackAnimPath;
	};

	// データメンバの宣言 -----------------------------------------------
private:


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EnemyInfoLoader()
	{}

	// デストラクタ
	~EnemyInfoLoader()
	{}


// 操作
public:
	/**
	 * @brief ファイル読み込み
	 *
	 * @param[in] path		 ファイルパス
	 * @param[in] outputInfo 情報出力用
	 *
	 * @return 読み込みに成功/失敗
	 */
	bool LoadData(const std::string& path, std::vector<EnemyInfo>& outputInfo)
	{
		using namespace DirectX;

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
			if (element.contains("speed") && element["speed"].is_number())
			{
				data.speed = element["speed"];
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

			// 攻撃
			if (element.contains("attack") && element["attack"].is_array())
			{
				data.attack = AttackDesc
				{
					element["attack"][0].get<float>(),
					element["attack"][1].get<float>(),
					element["attack"][2].get<float>()
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
