/**
 * @file   TargetBox.cpp
 *
 * @brief  的に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TargetBox.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/Effect/Area/AreaEffect.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pUR ユーザーリソースのポインタ
 */
TargetBox::TargetBox(UserResources* pUR)
	: m_collisionHandle{ 0 }
	, m_model(nullptr)
	, m_isHit(false)
{
	ID3D11DeviceContext* context = pUR->GetDeviceResources()->GetD3DDeviceContext();
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);

	// エフェクトの作成
	m_effect = std::make_unique<AreaEffect>(pUR);
}



/**
 * @brief デストラクタ
 */
TargetBox::~TargetBox()
{
	m_effect.reset();
}



/**
 * @brief 初期化処理
 *
 * @param pRM			リソースマネージャーのポインタ
 * @param pCM			当たり判定マネージャーのポインタ
 * @param pEM			敵マネージャーのポインタ
 * @param operation		ぶつけた時の処理
 * @param position		位置
 * @param halfLength	大きさの半分
 * @param angle			回転角度
 *
 * @return なし
 */
void TargetBox::Initialize(
	ResourceManager* pRM,
	CollisionManager* pCM,
	EnemyManager* pEM,
	const std::function<void()>& operation,
	const DirectX::SimpleMath::Vector3& position,
	const DirectX::SimpleMath::Vector3& halfLength,
	const DirectX::SimpleMath::Vector3& angle)
{
	// 位置・大きさ(1辺の半分)・回転を設定
	m_position = position;
	m_halfLength = halfLength;
	m_angle = angle;
	// 当たったかどうかの判定を初期化
	m_isHit = false;

	// モデルの読み込み
	m_model = pRM->RequestSDKMESH("box", "box.sdkmesh");

	// ぶつけた時の処理
	m_operation = operation;

	// エフェクトの設定
	m_effect->SetPosition(DirectX::SimpleMath::Vector3(m_position.x, m_position.y - m_halfLength.y, m_position.z));
	m_effect->SetLength(DirectX::SimpleMath::Vector3(m_halfLength.x * 2.5f, m_halfLength.y * 1.5f, m_halfLength.z * 2.5f));
	m_effect->SetColor(DirectX::Colors::Yellow.v);

	// 当たり判定の作成
	m_collider.SetCenter(m_position);
	m_collider.SetHalfLength(m_halfLength);
	float rotX = DirectX::XMConvertToRadians(m_angle.x);
	float rotY = DirectX::XMConvertToRadians(m_angle.y);
	float rotZ = DirectX::XMConvertToRadians(m_angle.z);
	m_collider.SetRotation(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rotY, rotX, rotZ));

	// 当たり判定マネージャーに登録
	CollisionManager::Desc desc{};
	desc.type = CollisionManager::Type::OBB;
	desc.layer = CollisionManager::Layer::Stage;
	desc.obb = &m_collider;
	desc.position = nullptr;
	desc.velocity = nullptr;
	desc.restitution = 0.2f;
	desc.callback.onEnter =
		[this, pCM, pEM](uint32_t, uint32_t other)
		{
			// 当たったのが敵本体なら通す
			if (pCM->GetDesc(other)->layer != CollisionManager::Layer::EnemyBody) return;

			// IDから敵を取得
			IEnemy* enemy = pEM->GetEnemyByID(pCM->GetDesc(other)->userId);

			// 当たった敵が跳ね返り状態なら処理を呼び出す
			if (enemy->GetStateType() == StateType::Bounce && !m_isHit)
			{
				m_operation();
				m_isHit = true;
			}
		};
	m_collisionHandle = pCM->Add(desc);
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void TargetBox::Update(float elapsedTime)
{
	if (!m_isHit)
	{
		// エフェクトの更新
		m_effect->Update(elapsedTime);
	}
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void TargetBox::Draw(const RenderContext& context)
{
	// ワールド行列の作成
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_halfLength * 2.0f);
	//scale = DirectX::SimpleMath::Matrix::CreateScale(1.f);
	float rotX = DirectX::XMConvertToRadians(m_angle.x);
	float rotY = DirectX::XMConvertToRadians(m_angle.y);
	float rotZ = DirectX::XMConvertToRadians(m_angle.z);
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateRotationX(rotX) *
		DirectX::SimpleMath::Matrix::CreateRotationY(rotY) *
		DirectX::SimpleMath::Matrix::CreateRotationZ(rotZ);
	world = scale * rot * trans;

	// モデルの描画
	m_model->Draw(context.deviceContext, *context.states, world, context.view, context.proj);

	if (!m_isHit)
	{
		// エフェクトの描画
		m_effect->Draw(context);
	}

	// 当たり判定の描画(デバッグ用)
	//m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Yellow);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void TargetBox::Finalize()
{
	m_model = nullptr;
}