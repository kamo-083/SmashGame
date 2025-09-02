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
void StageManager::CreateStage(const UserResources* pUR, const CollisionManager* pCM)
{

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
	m_goal->Draw(context, debugFont);
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

}
