/**
 * @file   Goal.cpp
 *
 * @brief  ゴールに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Goal.h"
#include "Source/Game/Common/CollisionManager.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/Scenes/StageScene.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param context	デバイスコンテキストのポインタ
 * @param pScene	シーンのポインタ
 */
Goal::Goal(ID3D11DeviceContext* context, StageScene* pScene)
	: m_position{ DirectX::SimpleMath::Vector3::Zero }
	, m_goalCollider{}
	, m_collisionHandle{ 0 }
	, m_isGoal{ false }
	, m_canGoal{ false }
	, m_pScene{ pScene }
	, m_tweenParam{}
{
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
}



/**
 * @brief デストラクタ
 */
Goal::~Goal()
{
	m_geometricPrimitive.reset();
}



/**
 * @brief 初期化処理
 *
 * @param pRM		リソースマネージャーのポインタ
 * @param pCM		当たり判定マネージャーのポインタ
 * @param position	位置
 *
 * @return なし
 */
void Goal::Initialize(
	ResourceManager* pRM,
	CollisionManager* pCM, 
	DirectX::SimpleMath::Vector3 position)
{
	// 位置の設定
	m_position = position;

	// モデルの取得
	m_models = std::make_unique<Models>();
	m_models->fishOnTable = pRM->RequestSDKMESH("fishTable", "fish.sdkmesh");
	m_models->cageLid = pRM->RequestSDKMESH("cageLid", "lid.sdkmesh");

	// 当たり判定の作成
	// テーブル
	m_tableCollider.SetCenter(m_position);
	m_tableCollider.SetRotation(DirectX::SimpleMath::Quaternion::Identity);
	m_tableCollider.SetHalfLength(DirectX::SimpleMath::Vector3(TABLE_HALF_LENGTH));

	// 当たり判定マネージャーに登録
	CollisionManager::Desc desc{};
	desc.type = CollisionManager::Type::OBB;
	desc.layer = CollisionManager::Layer::Stage;
	desc.obb = &m_tableCollider;
	desc.position = nullptr;
	desc.velocity = nullptr;
	m_collisionHandle = pCM->Add(desc);

	// ゴール
	m_goalCollider.SetCenter(m_position);
	m_goalCollider.SetRotation(DirectX::SimpleMath::Quaternion::Identity);
	m_goalCollider.SetHalfLength(DirectX::SimpleMath::Vector3(GOAL_HALF_LENGTH));

	// 当たり判定マネージャーに登録
	desc.layer = CollisionManager::Layer::Trigger;
	desc.obb = &m_goalCollider;
	desc.isTrigger = true;
	desc.callback.onEnter =
		[this, pCM](uint32_t, uint32_t other)
		{
			if (!m_canGoal || pCM->GetDesc(other)->layer != CollisionManager::Layer::PlayerBody) return;

			m_isGoal = true;	// ゴールした
		};
	m_collisionHandle = pCM->Add(desc);

	// トゥイーンアニメーションの設定
	m_tweenParam = {
		m_position,
		DirectX::SimpleMath::Vector3(GOAL_HALF_LENGTH),
		DirectX::SimpleMath::Quaternion::Identity,
		1.0f
	};
	Tween3D::TweenData data = {
		m_tweenParam,
		{DirectX::SimpleMath::Vector3(0.0f, LID_MOVE_Y, 0.0f), DirectX::SimpleMath::Vector3::Zero,
		 DirectX::SimpleMath::Quaternion::Identity, -1.0f},
		 TWEEN_ANIM_TIME,
		 Tween3D::Ease::InQuart,
		 Tween3D::PlaybackMode::Once
	};
	m_tweenAnim = std::make_unique<Tween3D>(data);

	// フラグの初期化
	m_canGoal = false;
	m_isGoal = false;
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void Goal::Update(float elapsedTime)
{
	// トゥイーンアニメーションの更新
	if (m_canGoal) m_tweenAnim->Update(elapsedTime, m_tweenParam);
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 * @param debugFont	デバッグ用フォント
 *
 * @return なし
 */
void Goal::Draw(RenderContext& context, DebugFont* debugFont)
{
	// モデルの描画
	// サカナのテーブル
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	DirectX::SimpleMath::Matrix world = trans;
	m_models->fishOnTable->Draw(context.deviceContext, *context.states, world, context.view, context.proj);
	
	// フタ
	if (!m_tweenAnim->Finished())
	{
		trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_tweenParam.pos);
		world = trans;
		m_models->cageLid->Draw(context.deviceContext, *context.states, world, context.view, context.proj);
	}

	// 当たり判定のデバッグ描画
	//DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(GOAL_HALF_LENGTH * 2.0f);
	//world = scale * trans;
	//m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Aqua, nullptr, true);

	// デバッグ情報の追加
	debugFont->AddString(0, 140, DirectX::Colors::Yellow, L" can = %d", m_canGoal);
	debugFont->AddString(100, 140, DirectX::Colors::Yellow, L"goal = %d", m_isGoal);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Goal::Finalize()
{
	m_pScene = nullptr;
	m_geometricPrimitive.reset();
	m_models.reset();
}



/**
 * @brief ゴール可能/不可能の設定
 *
 * @param canGoal	ゴール可/不可
 *
 * @return なし
 */
void Goal::CanGoal(bool canGoal)
{
	if (!m_canGoal && canGoal)
	{
		// SEの再生
		m_pScene->PlaySE("canGoalSE");
		// トゥイーンアニメーションの再生
		m_tweenAnim->Play();
	}

	// ゴール可能フラグを更新
	m_canGoal = canGoal;
}