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
 * @param[in] なし
 *
 * @return なし
 */
void StageManager::CreateStage(UserResources* pUR, CollisionManager* pCM, EnemyManager* pEM,
							   const std::string& path)
{
	StageLoader loader;
	std::vector<StageLoader::ObjectData> objectData;

	// データの読み込み
	loader.LoadData(path, objectData);

	ID3D11DeviceContext* context = pUR->GetDeviceResources()->GetD3DDeviceContext();

	for (StageLoader::ObjectData data : objectData)
	{
		switch (data.type)
		{
			// 地面
		case StageLoader::ObjectType::Ground:
		{
			m_grounds.push_back(std::move(std::make_unique<Ground>(context, data.position, data.scale)));
			m_grounds;
			break;
		}
		// 吹っ飛ぶ箱
		case StageLoader::ObjectType::BounceBox:
		{
			std::unique_ptr<BounceBox> bounce = std::make_unique<BounceBox>(context);
			bounce->Initialize(pCM, data.position, data.scale);
			m_bounceBoxes.push_back(std::move(bounce));
			break;
		}
		// 的の箱
		case StageLoader::ObjectType::TargetBox:	// とりあえずgoal.getしてるけど例外出ると思うので要対策
		{
			std::unique_ptr<TargetBox> target = std::make_unique<TargetBox>(context);
			target->Initialize(pCM, pEM, m_goal.get(), data.position, data.scale);
			m_targetBoxes.push_back(std::move(target));
			break;
		}
		// エリア
		case StageLoader::ObjectType::Area:	// スケール以降の引数は仮で入れているので後から修正
		{
			std::unique_ptr<CountArea> area = std::make_unique<CountArea>(context);
			area->Initialize(pCM, data.position, data.scale.x, data.scale.z,
							 [](){},CountArea::TriggerMode::AllOut);
			m_areas.push_back(std::move(area));
			break;
		}
		// ゴール
		case StageLoader::ObjectType::Goal:
		{
			m_goal = std::make_unique<Goal>(context);
			m_goal->Initialize(data.position);
			break;
		}

		}
	}
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
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
	m_goal->Update();
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
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
	m_goal->Finalize();

}
