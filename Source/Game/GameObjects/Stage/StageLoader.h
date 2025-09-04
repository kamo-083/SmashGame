/**
 * @file   StageLoader.h
 *
 * @brief  StageLoaderに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <sstream>

using json = nlohmann::json;


// クラスの定義 ===============================================================
/**
 * @brief StageLoader
 */
class StageLoader
{
	// クラス定数の宣言 -------------------------------------------------
public:
	enum class ObjectType
	{
		None = -1,
		Goal,
		Ground,
		BounceBox,
		TargetBox,
		Area,
	};

	struct AreaActionDesc
	{
		std::string mode;
		std::string command;
		int target = 0;
	};

	struct ObjectData
	{
		ObjectType type = ObjectType::None;
		DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
		DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3::One;
		bool active = true;
		AreaActionDesc areaAction;
	};
	
	struct EnemyData
	{
		std::string type;
		DirectX::SimpleMath::Vector3 position;
	};

	// データメンバの宣言 -----------------------------------------------
private:


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageLoader()
	{}

	// デストラクタ
	~StageLoader()
	{}


// 操作
public:
	bool LoadData(const std::string& path, std::vector<ObjectData>& outputObjects, std::vector<EnemyData>& outputEnemies)
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
		// ステージオブジェクト
		outputObjects.clear();
		for (auto& element : j["objects"])
		{
			ObjectData data;

			// 種類
			std::string typeStr = element.value("type", "Ground");
			if (typeStr == "Ground") data.type = ObjectType::Ground;
			else if (typeStr == "BounceBox") data.type = ObjectType::BounceBox;
			else if (typeStr == "TargetBox") data.type = ObjectType::TargetBox;
			else if (typeStr == "Area") data.type = ObjectType::Area;
			else if (typeStr == "Goal") data.type = ObjectType::Goal;

			// 座標
			if (element.contains("pos") && element["pos"].is_array() && element["pos"].size() >= 3)
			{
				data.position = SimpleMath::Vector3
				{
					element["pos"][0].get<float>(),
					element["pos"][1].get<float>(),
					element["pos"][2].get<float>()
				};
			}

			// 大きさ
			if (element.contains("scale") && element["scale"].is_array() && element["scale"].size() >= 3)
			{
				data.scale = SimpleMath::Vector3
				{
					element["scale"][0].get<float>(),
					element["scale"][1].get<float>(),
					element["scale"][2].get<float>()
				};
			}

			// 操作
			if (data.type == ObjectType::Area)
			{
				data.areaAction.mode = element.value("mode", "AllOut");
				data.areaAction.command = element.value("command", "EnableGoal");
				data.areaAction.target = element["target"];
			}

			// 有効化
			data.active = element.value("active", true);

			// 配列に追加
			outputObjects.push_back(std::move(data));
		}

		// 敵
		outputEnemies.clear();
		for (auto& element : j["enemies"])
		{
			EnemyData data;

			// 種類
			std::string typeStr = element.value("type", "Ground");
			data.type = typeStr;

			// 座標
			if (element.contains("pos") && element["pos"].is_array() && element["pos"].size() >= 3)
			{
				data.position = SimpleMath::Vector3
				{
					element["pos"][0].get<float>(),
					element["pos"][1].get<float>(),
					element["pos"][2].get<float>()
				};
			}

			// 配列に追加
			outputEnemies.push_back(std::move(data));
		}

		return true;
	}
};
