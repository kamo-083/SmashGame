/**
 * @file   StageManager.cpp
 *
 * @brief  ステージマネージャーに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageManager.h"
#include "Source/Game/Scenes/StageScene.h"
#include "Source/Game/GameObjects/Stage/Objects/Goal.h"
#include "Source/Game/GameObjects/Stage/Objects/Ground.h"
#include "Source/Game/GameObjects/Stage/Objects/TargetBox.h"
#include "Source/Game/GameObjects/Stage/Objects/CountArea.h"
#include "Source/Game/GameObjects/Stage/Objects/Fence.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pScene	シーンのポインタ
 */
StageManager::StageManager(StageScene* pScene)
	:m_pScene{ pScene }
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
	m_goal.reset();
	m_pScene = nullptr;
}



/**
 * @brief ステージ生成
 *
 * @param pUR  ユーザーリソースのポインタ
 * @param pCM  当たり判定マネージャーのポインタ
 * @param pEM  敵マネージャーのポインタ
 * @param path ステージのファイルパス
 *
 * @return なし
 */
void StageManager::CreateStage(UserResources* pUR, CollisionManager* pCM, EnemyManager* pEM,
							   const std::string& path)
{
	StageLoader loader;
	std::vector<StageLoader::ObjectData> objectData;
	std::vector<StageLoader::EnemyData> enemyData;

	// データの読み込み
	loader.LoadData(path, objectData, enemyData);

	ID3D11DeviceContext* context = pUR->GetDeviceResources()->GetD3DDeviceContext();

	// ステージオブジェクトの生成
	for (StageLoader::ObjectData data : objectData)
	{
		switch (data.type)
		{
		// 地面
		case StageLoader::ObjectType::Ground:
		{
			m_grounds.push_back(std::move(std::make_unique<Ground>(context)));
			m_grounds.back()->Initialize(pCM, data.position, data.scale);
			break;
		}
		// 的の箱
		case StageLoader::ObjectType::TargetBox:
		{
			m_targetBoxes.push_back(std::move(std::make_unique<TargetBox>(context)));
			m_targetBoxes.back()->Initialize(pCM, pEM, m_goal.get(), data.position, data.scale);
			break;
		}
		// エリア
		case StageLoader::ObjectType::Area:
		{
			// 操作を設定
			std::function<void()> operate;
			CreateOperate(operate, data.areaAction);

			// モードを設定
			CountArea::TriggerMode mode{};
			if (data.areaAction.mode == "AllOut") mode = CountArea::TriggerMode::AllOut;
			else if (data.areaAction.mode == "ReachCount") mode = CountArea::TriggerMode::ReachCount;

			m_areas.push_back(std::move(std::make_unique<CountArea>(pUR)));
			m_areas.back()->Initialize(
				pCM, data.position, data.scale.x, data.scale.z,
				operate, mode, data.areaAction.target);
			break;
		}
		// 柵
		case StageLoader::ObjectType::Fence:
		{
			m_fences.push_back(std::move(std::make_unique<Fence>(context)));
			m_fences.back()->Initialize(
				pUR->GetResourceManager(), pCM, data.fenceNum,
				data.position, data.scale, data.angle);
			break;
		}
		// ゴール
		case StageLoader::ObjectType::Goal:
		{
			m_goal = std::make_unique<Goal>(context, m_pScene);
			m_goal->Initialize(pCM, data.position);
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

		pEM->Spawn(spawnData);
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
void StageManager::Update(float elapsedTime, DirectX::SimpleMath::Vector3 cameraPos, DirectX::SimpleMath::Vector3 cameraUp)
{
	// 地面の更新
	for (auto& ground : m_grounds)
	{
		ground->Update();
	}

	// 的の更新
	for (auto& targetBox : m_targetBoxes)
	{
		targetBox->Update(elapsedTime);
	}

	// エリアの更新
	for (auto& area : m_areas)
	{
		area->Update(cameraPos, cameraUp);
	}
	
	// 柵の更新
	for (auto& fences : m_fences)
	{
		fences->Update();
	}

	// ゴールの更新
	if(m_goal) m_goal->Update();
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 * @param debugFont デバッグ用フォント
 *
 * @return なし
 */
void StageManager::Draw(RenderContext context, Imase::DebugFont* debugFont)
{
	// 地面の描画
	for (auto& ground : m_grounds)
	{
		ground->Draw(context);
	}

	// 的の描画
	for (auto& targetBox : m_targetBoxes)
	{
		targetBox->Draw(context);
	}

	// 柵の描画
	for (auto& fences : m_fences)
	{
		fences->Draw(context);
	}

	// ゴールの描画
	if(m_goal) m_goal->Draw(context, debugFont);
}



/**
 * @brief 描画処理(半透明オブジェクト)
 *
 * @param context	描画用構造体
 * @param debugFont デバッグ用フォント
 *
 * @return なし
 */
void StageManager::DrawTranslucent(RenderContext context, Imase::DebugFont* debugFont)
{
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

	// ゴールの終了
	if(m_goal) m_goal->Finalize();
}



/**
 * @brief ギミック処理生成
 *
 * @param outOperate 処理出力用
 * @param desc		 ギミック情報
 *
 * @return なし
 */
void StageManager::CreateOperate(std::function<void()>& outOperate, StageLoader::AreaActionDesc& desc)
{
	if (desc.command == "EnableGoal")
	{
		outOperate = [this]()
			{
				m_goal->CanGoal();
			};

		return;
	}
}



/**
 * @brief ゴールしているか
 *
 * @param なし
 *
 * @retval true  ゴールしている
 * @retval false ゴールしていない
 */
bool StageManager::IsGoal()
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
bool StageManager::IsCanGoal()
{
	return m_goal->IsCanGoal();
}
