/**
 * @file   StageLoader.h
 *
 * @brief  StageLoaderに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Source/Game/Data/StageEventData.h"
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
	// ステージオブジェクトの種類
	enum class ObjectType
	{
		None = -1,
		Goal,		// ゴール
		Ground,		// 地面
		TargetBox,	// 的
		Area,		// エリア
		Fence,		// 柵
		Bridge,		// 橋
		Key,		// 鍵
	};

	// エリア設定情報
	struct AreaActionDesc
	{
		std::string mode;		// 条件
		std::string command;	// 条件達成時の処理
		int target = 0;			// 目標値(条件に必要な時は入力)
	};

	// ステージオブジェクトの情報
	struct ObjectData
	{
		std::string objectID;					// オブジェクトID
		ObjectType type = ObjectType::None;		// 種類
		DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;	// 座標
		DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3::One;		// 大きさ
		DirectX::SimpleMath::Vector3 angle = DirectX::SimpleMath::Vector3::Zero;	// 角度
		std::vector<EventHandle> events;	// イベント
		bool active = true;					// 有効化フラグ
		AreaActionDesc areaAction;			// エリア設定(CountAreaの時)
		int fenceNum = 0;					// 柵を並べる数(Fenceの時)
		float bridgeAngle = 0.0f;			// 橋の向き(Bridgeの時)
	};
	
	// 敵の情報
	struct EnemyData
	{
		std::string type;						// 種類
		DirectX::SimpleMath::Vector3 position;	// 座標
		bool dropRespawn = false;				// 落下時のリスポーン有無
	};

// メンバ関数の定義 -------------------------------------------------
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
	/**
	 * @brief ファイル読み込み
	 *
	 * @param path			ファイルパス
	 * @param outputObjects オブジェクト情報出力用
	 * @param outputEnemies 敵情報出力用
	 *
	 * @return 読み込みに成功/失敗
	 */
	bool LoadData(const std::string& path, std::vector<ObjectData>& outputObjects, std::vector<EnemyData>& outputEnemies)
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
		// ステージオブジェクト
		outputObjects.clear();
		for (auto& element : j["objects"])
		{
			ObjectData data;

			// オブジェクトID
			if (element.contains("objectID") && element["objectID"].is_string())
			{
				data.objectID = element["objectID"].get<std::string>();
			}

			// 種類
			if (element.contains("type") && element["type"].is_string())
			{
				std::string typeStr = element.value("type", "Ground");
				if (typeStr == "Ground") data.type = ObjectType::Ground;
				else if (typeStr == "TargetBox") data.type = ObjectType::TargetBox;
				else if (typeStr == "Area") data.type = ObjectType::Area;
				else if (typeStr == "Goal") data.type = ObjectType::Goal;
				else if (typeStr == "Fence") data.type = ObjectType::Fence;
				else if (typeStr == "Bridge") data.type = ObjectType::Bridge;
				else if (typeStr == "Key") data.type = ObjectType::Key;
			}

			// 座標
			if (element.contains("pos") && element["pos"].is_array() && element["pos"].size() >= 3)
			{
				data.position = DirectX::SimpleMath::Vector3
				{
					element["pos"][0].get<float>(),
					element["pos"][1].get<float>(),
					element["pos"][2].get<float>()
				};
			}

			// 大きさ
			if (element.contains("scale") && element["scale"].is_array() && element["scale"].size() >= 3)
			{
				data.scale = DirectX::SimpleMath::Vector3
				{
					element["scale"][0].get<float>(),
					element["scale"][1].get<float>(),
					element["scale"][2].get<float>()
				};
			}

			// 操作
			if (data.type == ObjectType::Area && 
				(element.contains("mode") && element["mode"].is_string()) &&
				(element.contains("command") && element["command"].is_string()) &&
				(element.contains("target") && element["target"].is_number()))
			{
				data.areaAction.mode = element.get<std::string>();
				data.areaAction.command = element.get<std::string>();
				data.areaAction.target = element.get<int>();
			}

			// 柵の回転・数
			if (data.type == ObjectType::Fence)
			{
				// 回転
				if (element.contains("angle") && element["angle"].is_array() && element["angle"].size() >= 3)
				{
					data.angle = DirectX::SimpleMath::Vector3
					{
						element["angle"][0].get<float>(),
						element["angle"][1].get<float>(),
						element["angle"][2].get<float>()
					};
				}
				// 数
				if (element.contains("num") && element["num"].is_number())
				{
					data.fenceNum = element["num"].get<int>();
				}
			}
			
			// 橋の向き
			if (data.type == ObjectType::Bridge && element.contains("angle") && element["angle"].is_number())
			{
				// 回転
				data.bridgeAngle = element["angle"].get<float>();
			}

			// イベント
			if (element.contains("events") && element["events"].is_array())
			{
				std::vector<EventHandle> events;
				for (auto& eventElement : element["events"])
				{
					EventHandle handle;

					// 対象のオブジェクトID
					if (eventElement.contains("targetID") && eventElement["targetID"].is_string())
					{
						handle.objectID = eventElement["targetID"].get<std::string>();
					}
					// イベントの種類
					if (eventElement.contains("event") && eventElement["event"].is_string())
					{
						handle.event = ToStageEvent(eventElement["event"].get<std::string>());
					}
					events.push_back(handle);
				}
				// イベントを登録
				data.events = events;
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
				data.position = DirectX::SimpleMath::Vector3
				{
					element["pos"][0].get<float>(),
					element["pos"][1].get<float>(),
					element["pos"][2].get<float>()
				};
			}

			// 落下時のリスポーン有無
			if (element.contains("dropRespawn") && element["dropRespawn"].is_boolean())
			{
				data.dropRespawn = element["dropRespawn"];
			}

			// 配列に追加
			outputEnemies.push_back(std::move(data));
		}

		return true;
	}
};