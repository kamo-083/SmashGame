/**
 * @file   StageManager.cpp
 *
 * @brief  StageManagerに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageManager.h"
#include "Source/Game/GameObjects/Stage/Objects/Ground.h"
#include "Source/Game/GameObjects/Stage/Objects/BounceBox.h"
#include "Source/Game/GameObjects/Stage/Objects/TargetBox.h"
#include "Source/Game/GameObjects/Stage/Objects/Goal.h"
#include "Source/Game/GameObjects/Stage/Objects/CountArea.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
StageManager::StageManager()
{

}



/**
 * @brief デストラクタ
 */
StageManager::~StageManager()
{

}



/**
 * @brief ステージ生成
 *
 * @param[in] pUR  ユーザーリソースのポインタ
 * @param[in] pCM  コリジョンマネージャーのポインタ
 * @param[in] pEM  エネミーマネージャーのポインタ
 * @param[in] path ステージのパスファイル
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
		// 吹っ飛ぶ箱
		case StageLoader::ObjectType::BounceBox:
		{
			m_bounceBoxes.push_back(std::move(std::make_unique<BounceBox>(context)));
			m_bounceBoxes.back()->Initialize(pCM, data.position, data.scale);
			break;
		}
		// 的の箱
		case StageLoader::ObjectType::TargetBox:	// とりあえずgoal.getしてるけど順番によっては例外出ると思うので要対策
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

			m_areas.push_back(std::move(std::make_unique<CountArea>(context)));
			m_areas.back()->Initialize(pCM, data.position, data.scale.x, data.scale.z,
									   operate, mode);
			break;
		}
		// ゴール
		case StageLoader::ObjectType::Goal:
		{
			m_goal = std::make_unique<Goal>(context);
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
		spawnData.position = data.position;

		pEM->Spawn(spawnData);
	}
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void StageManager::Update(float elapsedTime)
{
	// 地面の更新
	for (auto& ground : m_grounds)
	{
		ground->Update();
	}

	// 箱の更新
	for (auto& bounceBox : m_bounceBoxes)
	{
		bounceBox->Update(elapsedTime);
	}

	// 的の更新
	for (auto& targetBox : m_targetBoxes)
	{
		targetBox->Update(elapsedTime);
	}

	// エリアの更新
	for (auto& area : m_areas)
	{
		area->Update();
	}

	// ゴールの更新
	if(m_goal) m_goal->Update();
}



/**
 * @brief 描画処理
 *
 * @param[in] context	描画用構造体
 * @param[in] debugFont デバッグ用フォント
 *
 * @return なし
 */
void StageManager::Draw(RenderContext context, Imase::DebugFont* debugFont)
{
	// 地面の更新
	for (auto& ground : m_grounds)
	{
		ground->Draw(context);
	}

	// 箱の更新
	for (auto& bounceBox : m_bounceBoxes)
	{
		bounceBox->Draw(context, debugFont);
	}

	// 的の更新
	for (auto& targetBox : m_targetBoxes)
	{
		targetBox->Draw(context);
	}

	// エリアの更新
	for (auto& area : m_areas)
	{
		area->Draw(context, debugFont);
	}

	// ゴールの更新
	if(m_goal) m_goal->Draw(context, debugFont);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
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

	// 箱の終了
	for (auto& bounceBox : m_bounceBoxes)
	{
		bounceBox->Finalize();
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

	// ゴールの終了
	if(m_goal) m_goal->Finalize();

}



/**
 * @brief ギミック処理生成
 *
 * @param[in] outOperate 処理出力用
 * @param[in] desc		 ギミック情報
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
 * @brief ゴールしているかを返す
 *
 * @param[in] なし
 *
 * @return ゴールしているか
 */
bool StageManager::IsGoal()
{
	return m_goal->IsGoal();
}
