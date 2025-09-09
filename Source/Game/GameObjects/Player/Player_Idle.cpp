/**
 * @file   Player_Idle.cpp
 *
 * @brief  Player_Idleに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_Idle.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] player	プレイヤーのポインタ
 * @param[in] kbTracker キーボードトラッカーのポインタ
 */
Player_Idle::Player_Idle(Player* player, DirectX::Keyboard::KeyboardStateTracker* kbTracker)
	: m_pPlayer{ player }
	, m_pKbTracker{ kbTracker }
	, m_model{ nullptr }
	, m_stateType{ StateType::Idle }
{

}


/**
 * @brief デストラクタ
 */
Player_Idle::~Player_Idle()
{

}


/**
 * @brief 初期化処理
 *
 * @param[in] pResourceManager  リソースマネージャーのポインタ
 *
 * @return なし
 */
void Player_Idle::Initialize(ResourceManager* pResourceManager)
{
	m_model = pResourceManager->RequestSDKMESH("player", L"Resources/Models/player.sdkmesh");

	// アニメーションを取得
	m_animation = m_pPlayer->GetAnimation()->idle;

	// アニメーションとモデルをバインドする
	m_animation->Bind(*m_model);

	// アニメーションとモデルをバインドする
	m_animation->Bind(*m_model);

	// ボーン用のトランスフォーム配列を生成
	m_drawBones = DirectX::ModelBone::MakeArray(m_model->bones.size());
}


/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void Player_Idle::Update(const float& elapsedTime)
{
	// 座標の更新
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime, m_pPlayer->GetOnGround());
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	m_pPlayer->SetOnGround(false);

	// 武器の切り替え
	m_pPlayer->ChangeWeapon(m_pKbTracker);

	// 吹っ飛ばされ状態
	if (m_pPlayer->GetIsBounce())
	{
		if (m_pPlayer->GetVelocity().Length() < 1.0f)
		{
			m_pPlayer->SetIsBounce(false);
			m_pPlayer->GetTrajectoryParticle()->SetSpawn(false);
		}
		else return;
	}

	// アニメーションの更新
	m_animation->Update(elapsedTime);

	// 歩き状態に切り替え
	if (m_pKbTracker->GetLastState().W || m_pKbTracker->GetLastState().S || m_pKbTracker->GetLastState().A || m_pKbTracker->GetLastState().D)
	{
		m_pPlayer->ChangeState(m_pPlayer->GetState_Walk());
	}

	// 攻撃状態に切り替え
	if (m_pKbTracker->pressed.Space)
	{
		m_pPlayer->Attack();
	}
}


/**
 * @brief 描画処理
 *
 * @param[in] context	描画用構造体
 *
 * @return なし
 */
void Player_Idle::Render(RenderContext& context)
{
	SimpleMath::Matrix world;
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationY(m_pPlayer->GetRotY());
	SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(m_pPlayer->GetScale());
	world = scale * rot * trans;

	//m_model->Draw(context.deviceContext, *context.states, world, context.view, context.projection, m_pPlayer->GetIsBounce());

	// ボーン数を取得する
	size_t nbones = m_model->bones.size();

	// アニメーションにモデルを適用する
	m_animation->Apply(*m_model, nbones, m_drawBones.get());

	// アニメーションモデルを描画する
	m_model->DrawSkinned(
		context.deviceContext,
		*context.states, nbones,
		m_drawBones.get(),
		world,
		context.view,
		context.projection
	);
}


/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Player_Idle::Finalize()
{
	m_model = nullptr;
	m_animation = nullptr;
}
