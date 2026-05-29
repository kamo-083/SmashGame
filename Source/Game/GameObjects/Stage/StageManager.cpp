/**
 * @file   StageManager.cpp
 *
 * @brief  ステージマネージャーに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageManager.h"
#include "Source/Debug/DebugFont.h"
#include "Source/Game/Scenes/StageScene.h"
#include "Source/Game/Common/UserResources.h"
#include "Source/Game/Effect/EffectManager.h"
#include "Source/Game/Physics/CollisionManager.h"
#include "Source/Game/GameObjects/Enemy/EnemyManager.h"
#include "Source/Game/Common/AudioManager.h"
#include "Source/Game/GameObjects/Stage/StageObject.h"
#include "Source/Game/GameObjects/Stage/Objects/Goal.h"
#include "Source/Game/GameObjects/Stage/Objects/Ground.h"
#include "Source/Game/GameObjects/Stage/Objects/TargetBox.h"
#include "Source/Game/GameObjects/Stage/Objects/CountArea.h"
#include "Source/Game/GameObjects/Stage/Objects/Fence.h"
#include "Source/Game/GameObjects/Stage/Objects/Bridge.h"
#include "Source/Game/GameObjects/Stage/Objects/Key.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pDepthStencilState	深度ステンシルステートのポインタ
 */
StageManager::StageManager(ID3D11DepthStencilState* pDepthStencilState)
	: m_depthStencilState{ pDepthStencilState }
{
}

/**
 * @brief デストラクタ
 */
StageManager::~StageManager()
{
	m_grounds.clear();
	m_targetBoxes.clear();
	m_areas.clear();
	m_fences.clear();
	m_bridges.clear();
	m_goal.reset();
}

/**
 * @brief ステージ生成
 *
 * @param pUR  ユーザーリソースのポインタ
 * @param pCM  当たり判定マネージャーのポインタ
 * @param pEnM 敵マネージャーのポインタ
 * @param pEfM エフェクトマネージャーのポインタ
 * @param path ステージのファイルパス
 *
 * @return なし
 */
void StageManager::CreateStage(
	UserResources* pUR, CollisionManager* pCM, EnemyManager* pEnM, EffectManager* pEfM,
	const std::string& path)
{

	// データの読み込み
	StageLoader loader;
	std::vector<StageLoader::ObjectData> objectData;
	std::vector<StageLoader::EnemyData> enemyData;
	loader.LoadData(path, objectData, enemyData);

	// よく使用するポインタを取得
	ResourceManager* pRM = pUR->GetResourceManager();
	AudioManager* pAM = pUR->GetAudioManager();
	ID3D11DeviceContext* context = pUR->GetDeviceResources()->GetD3DDeviceContext();

	// ステージオブジェクトの生成
	for (StageLoader::ObjectData data : objectData)
	{
		// イベントとオブジェクトIDの設定
		StageObject::StageObjectDesc desc =
		{
			data.objectID, data.events, this
		};
		switch (data.type)
		{
		// 地面
		case StageLoader::ObjectType::Ground:
		{
			// オブジェクトを作成
			m_grounds.push_back(std::move(std::make_unique<Ground>(desc, context, m_depthStencilState.Get(), pRM)));
			m_grounds.back()->Initialize(pCM, data.position, data.scale);
			// オブジェクトを登録
			m_stageObjectMap[desc.stageObject_ID] = m_grounds.back().get();
			break;
		}
		// 的
		case StageLoader::ObjectType::TargetBox:
		{
			// オブジェクトを作成
			m_targetBoxes.push_back(std::move(std::make_unique<TargetBox>(desc, pUR)));
			m_targetBoxes.back()->Initialize(pRM, pCM, pEnM, data.position, data.scale);
			// オブジェクトを登録
			m_stageObjectMap[desc.stageObject_ID] = m_targetBoxes.back().get();
			break;
		}
		// エリア
		case StageLoader::ObjectType::Area:
		{
			// モードを設定
			CountArea::TriggerMode mode{};
			if (data.areaAction.mode == "AllOut") mode = CountArea::TriggerMode::AllOut;
			else if (data.areaAction.mode == "ReachCount") mode = CountArea::TriggerMode::ReachCount;
			// オブジェクトを作成
			m_areas.push_back(std::move(std::make_unique<CountArea>(desc, pUR)));
			m_areas.back()->Initialize(
				pCM, data.position, data.scale.x, data.scale.z,
				mode, data.areaAction.target);
			// オブジェクトを登録
			m_stageObjectMap[desc.stageObject_ID] = m_areas.back().get();
			break;
		}
		// 柵
		case StageLoader::ObjectType::Fence:
		{
			// オブジェクトを作成
			m_fences.push_back(std::move(std::make_unique<Fence>(desc, context)));
			m_fences.back()->Initialize(
				pRM, pCM, data.fenceNum,
				data.position, data.scale, data.angle);
			// オブジェクトを登録
			m_stageObjectMap[desc.stageObject_ID] = m_fences.back().get();
			break;
		}
		// 橋
		case StageLoader::ObjectType::Bridge:
		{
			// オブジェクトを作成
			m_bridges.push_back(std::move(std::make_unique<Bridge>(desc, pUR)));
			m_bridges.back()->Initialize(pRM, pCM, pEnM, data.position, data.bridgeAngle);
			// オブジェクトを登録
			m_stageObjectMap[desc.stageObject_ID] = m_bridges.back().get();
			break;
		}
		// 鍵
		case StageLoader::ObjectType::Key:
		{
			// オブジェクトを作成
			m_keys.push_back(std::move(std::make_unique<Key>(desc, context, pRM, pEfM, pAM)));
			// オブジェクトを登録
			m_stageObjectMap[desc.stageObject_ID] = m_keys.back().get();
			break;
		}
		// ゴール
		case StageLoader::ObjectType::Goal:
		{
			// オブジェクトを作成
			m_goal = std::make_unique<Goal>(desc, context, pAM);
			m_goal->Initialize(pRM, pCM, data.position);
			// オブジェクトを登録
			m_stageObjectMap[desc.stageObject_ID] = m_goal.get();
			break;
		}
		}
	}

	// 敵の生成
	for (StageLoader::EnemyData data : enemyData)
	{
		EnemyManager::SpawnData spawnData;
		if (data.type == "Basic")	   spawnData.type = EnemyType::Basic;
		else if (data.type == "Heavy") spawnData.type = EnemyType::Heavy;
		else if (data.type == "Light") spawnData.type = EnemyType::Light;
		spawnData.position = data.position;
		spawnData.dropRespawn = data.dropRespawn;

		pEnM->Spawn(spawnData);
	}
}

/**
 * @brief 更新処理
 *
 * @param elapsedTime	経過時間
 * @param cameraPos		カメラ位置
 * @param cameraUp		カメラ上ベクトル
 *
 * @return なし
 */
void StageManager::Update(float elapsedTime, const DirectX::SimpleMath::Vector3& cameraPos, const DirectX::SimpleMath::Vector3& cameraUp)
{
	// 的の更新
	for (auto& box : m_targetBoxes)
	{
		box->Update(elapsedTime, cameraPos, cameraUp);
	}

	// エリアの更新
	for (auto& area : m_areas)
	{
		area->Update(elapsedTime, cameraPos, cameraUp);
	}
	
	// 橋の更新
	for (auto& bridge : m_bridges)
	{
		bridge->Update(elapsedTime);
	}

	// 鍵の更新
	for (auto& key : m_keys)
	{
		key->Update(elapsedTime);
	}

	// ゴールの更新
	if(m_goal) m_goal->Update(elapsedTime);
}

/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 * @param debugFont デバッグ用フォント
 *
 * @return なし
 */
void StageManager::Draw(const RenderContext& context, DebugFont* debugFont)
{
	// 地面の描画
	for (auto& ground : m_grounds)
	{
		ground->Draw(context, debugFont);
	}

	// 柵の描画
	for (auto& fences : m_fences)
	{
		fences->Draw(context, debugFont);
	}
	
	// 橋の描画
	for (auto& bridge : m_bridges)
	{
		bridge->Draw(context, debugFont);
	}

	// 鍵の描画
	for (auto& key : m_keys)
	{
		key->Draw(context, debugFont);
	}

	// ゴールの描画
	if(m_goal) m_goal->Draw(context, debugFont);

	// 的の描画
	for (auto& targetBox : m_targetBoxes)
	{
		targetBox->Draw(context, debugFont);
	}

	// エリアの描画
	for (auto& area : m_areas)
	{
		area->Draw(context, debugFont);
	}
}

/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void StageManager::Finalize()
{
	// 地面の終了
	for (auto& ground : m_grounds)
	{
		ground->Finalize();
	}

	// 的の終了
	for (auto& targetBox : m_targetBoxes)
	{
		targetBox->Finalize();
	}

	// エリアの終了
	for (auto& area : m_areas)
	{
		area->Finalize();
	}

	// 柵の終了
	for (auto& fences : m_fences)
	{
		fences->Finalize();
	}

	// 橋の終了
	for (auto& bridge : m_bridges)
	{
		bridge->Finalize();
	}

	// ゴールの終了
	if(m_goal) m_goal->Finalize();
}

/**
 * @brief イベントの受け取り
 *
 * @param event		イベントの情報
 * @param senderID	送り主のオブジェクトID
 *
 * @return なし
 */
void StageManager::DispatchEvent(const EventHandle& handle,	const std::string& senderID)
{
	// IDが存在しているかを確認
	auto it = m_stageObjectMap.find(handle.objectID);
	if (it == m_stageObjectMap.end()) return;

	StageObject::StageEventContext context =
	{
		handle.event, handle.objectID, senderID
	};

	// IDから見つけたオブジェクトのイベントを発生させる
	it->second->OnStageEvent(context);
}

/**
 * @brief オブジェクトの位置の受け渡し
 *
 * @param objectID	対象オブジェクトのID
 *
 * @return 対象オブジェクトの位置
 */
DirectX::SimpleMath::Vector3 StageManager::DispatchPosition(const std::string& objectID)
{
	return m_stageObjectMap.at(objectID)->GetPosition();
}

/**
 * @brief ゴールしているか
 *
 * @param なし
 *
 * @retval true  ゴールしている
 * @retval false ゴールしていない
 */
bool StageManager::IsGoal() const
{
	return m_goal->IsGoal();
}

/**
 * @brief ゴールできるか
 *
 * @param なし
 *
 * @retval true  ゴールできる
 * @retval false ゴールできない
 */
bool StageManager::IsCanGoal() const
{
	return m_goal->IsCanGoal();
}