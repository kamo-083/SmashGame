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

	// プレイヤーの情報
	struct PlayerInfo
	{
		float radius = 0.0f;				// 半径サイズ
		float mass = 0.0f;					// 質量
		float walk_speed = 0.0f;			// 移動速度(通常)
		float dash_speed = 0.0f;			// 移動速度(攻撃)
		float max_speed = 0.0f;				// 最高移動速度
		float static_friction = 0.0f;		// 静止摩擦係数
		float dynamic_friction = 0.0f;		// 動摩擦係数
		float heavy_cooldown = 0.0f;		// 重攻撃の硬直時間

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

// 操作
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

		PlayerInfo data;

		// 大きさ
		if (j.contains("radius") && j["radius"].is_number())
		{
			data.radius = j["radius"];
		}

		// 質量
		if (j.contains("mass") && j["mass"].is_number())
		{
			data.mass = j["mass"];
		}

		// 移動速度(通常)
		if (j.contains("walk_speed") && j["walk_speed"].is_number())
		{
			data.walk_speed = j["walk_speed"];
		}

		// 移動速度(攻撃)
		if (j.contains("dash_speed") && j["dash_speed"].is_number())
		{
			data.dash_speed = j["dash_speed"];
		}

		// 最高速度
		if (j.contains("max_speed") && j["max_speed"].is_number())
		{
			data.max_speed = j["max_speed"];
		}

		// 静止摩擦係数
		if (j.contains("static_friction") && j["static_friction"].is_number())
		{
			data.static_friction = j["static_friction"];
		}

		// 動摩擦係数
		if (j.contains("dynamic_friction") && j["dynamic_friction"].is_number())
		{
			data.dynamic_friction = j["dynamic_friction"];
		}

		// 動摩擦係数
		if (j.contains("heavy_cooldown") && j["heavy_cooldown"].is_number())
		{
			data.heavy_cooldown = j["heavy_cooldown"];
		}

		// 攻撃
		if (j.contains("attack") && j["attack"].is_array())
		{
			for (auto& atk : j["attack"])
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
		if (j.contains("modelPath") && j["modelPath"].is_string())
		{
			data.modelPath = j["modelPath"];
		}

		// 待機アニメーションのパス
		if (j.contains("idleAnimPath") && j["idleAnimPath"].is_string())
		{
			data.idleAnimPath = j["idleAnimPath"];
		}

		// 移動アニメーションのパス
		if (j.contains("walkAnimPath") && j["walkAnimPath"].is_string())
		{
			data.walkAnimPath = j["walkAnimPath"];
		}

		// 攻撃アニメーションのパス
		if (j.contains("attackAnimPath") && j["attackAnimPath"].is_array())
		{
			for (auto& anim : j["attackAnimPath"])
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


		return true;
	}
};