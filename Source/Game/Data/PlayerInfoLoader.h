/**
 * @file   PlayerInfoLoader.h
 *
 * @brief  プレイヤー情報用ローダーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include "Source/Game/Data/AttackData.h"

using json = nlohmann::json;


// クラスの定義 ===============================================================
/**
 * @brief プレイヤー情報用ローダー
 */
class PlayerInfoLoader
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
	};

	// 敵の情報
	struct PlayerInfo
	{
		float radius = 0.0f;				// 半径サイズ
		float mass = 0.0f;					// 質量
		float walk_speed = 0.0f;			// 移動速度(通常)
		float dash_speed = 0.0f;			// 移動速度(攻撃)
		float max_speed = 0.0f;				// 最高移動速度
		float static_friction = 0.0f;		// 静止摩擦係数
		float dynamic_friction = 0.0f;		// 動摩擦係数

		AttackDesc attack[static_cast<int>(AttackType::TYPE_NUM)]; // 攻撃情報

		std::string modelPath;		// モデルのファイルパス
		std::string idleAnimPath;	// 待機アニメーションのファイルパス
		std::string walkAnimPath;	// 移動アニメーションのファイルパス
		std::string attackAnimPath[static_cast<int>(AttackType::TYPE_NUM)];	// 攻撃アニメーションのファイルパス
	};


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PlayerInfoLoader()
	{}

	// デストラクタ
	~PlayerInfoLoader() = default;


public:
	/**
	 * @brief ファイル読み込み
	 *
	 * @param path		 ファイルパス
	 * @param outputInfo 情報出力用
	 *
	 * @return 読み込みに成功/失敗
	 */
	bool LoadData(const std::string& path, PlayerInfo& outputInfo)
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
		for (auto& element : j["info"])
		{
			PlayerInfo data;

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

			// 移動速度(通常)
			if (element.contains("walk_speed") && element["walk_speed"].is_number())
			{
				data.walk_speed = element["walk_speed"];
			}

			// 移動速度(攻撃)
			if (element.contains("dash_speed") && element["dash_speed"].is_number())
			{
				data.dash_speed = element["dash_speed"];
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
				for (auto& atk : element["attack"])
				{
					// 種類を特定
					int type = 0;
					if (atk.contains("atkType") && atk["atkType"].is_string())
					{
						if (atk["atkType"] == "Basic")		  type = static_cast<int>(AttackType::BASIC);
						else if (atk["atkType"] == "Heavy")	  type = static_cast<int>(AttackType::HEAVY);
						else if (atk["atkType"] == "Rolling") type = static_cast<int>(AttackType::ROLLING);
					}

					// 各パラメータを格納
					if (atk.contains("atkParam") && atk["atkParam"].is_array())
					{
						data.attack[type] = AttackDesc
						{
							atk["atkParam"][0].get<float>(),	// 攻撃時間
							atk["atkParam"][1].get<float>(),	// 判定サイズ
							atk["atkParam"][2].get<float>()		// 攻撃力
						};
					}
#ifdef _DEBUG
					// デバッグ情報
					char buf[256];
					sprintf_s(buf, "PLAYER_ATK type=%d time=%.2f size=%.2f force=%.2f \n",
						type, data.attack[type].time, data.attack[type].size, data.attack[type].force);
					OutputDebugStringA(buf);
#endif
				}				
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
			if (element.contains("attackAnimPath") && element["attackAnimPath"].is_array())
			{
				for (auto& anim : element["attackAnimPath"])
				{
					// 種類を特定
					int type = 0;
					if (anim.contains("atkType") && anim["atkType"].is_string())
					{
						if (anim["atkType"] == "Basic")			type = static_cast<int>(AttackType::BASIC);
						else if (anim["atkType"] == "Heavy")	type = static_cast<int>(AttackType::HEAVY);
						else if (anim["atkType"] == "Rolling")	type = static_cast<int>(AttackType::ROLLING);
					}

					if (anim.contains("atkAnim") && anim["atkAnim"].is_string())
					{
						data.attackAnimPath[type] = anim["atkAnim"];
					}
#ifdef _DEBUG
					// デバッグ情報
					char buf[256];
					sprintf_s(buf, "PLAYER_ANIM type=%d path=" + *data.attackAnimPath[type].data(), type);
					OutputDebugStringA(buf);
#endif
				}
			}

			// 情報を登録
			outputInfo = data;
		}

		return true;
	}
};
