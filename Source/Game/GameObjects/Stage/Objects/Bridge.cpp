/**
 * @file   Bridge.cpp
 *
 * @brief  橋に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Bridge.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/Effect/Area/AreaEffect.h"
#include "Source/Game/Interface/IState.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pUR ユーザーリソースのポインタ
 */
Bridge::Bridge(UserResources* pUR)
	: AudioListener{ pUR->GetAudioManager() }
	, m_collisionHandle{ 0 }
	, m_model(nullptr)
	, m_isDowned(false)
{
	ID3D11DeviceContext* context = pUR->GetDeviceResources()->GetD3DDeviceContext();
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);

	// エフェクトの作成
	m_effect = std::make_unique<AreaEffect>(pUR);
}



/**
 * @brief デストラクタ
 */
Bridge::~Bridge()
{
	m_effect.reset();
}



/**
 * @brief 初期化処理
 *
 * @param pRM			リソースマネージャーのポインタ
 * @param pCM			当たり判定マネージャーのポインタ
 * @param pEM			敵マネージャーのポインタ
 * @param position		位置
 * @param angle			橋の向き
 *
 * @return なし
 */
void Bridge::Initialize(
	ResourceManager* pRM,
	CollisionManager* pCM,
	EnemyManager* pEM,
	const DirectX::SimpleMath::Vector3& position,
	const float angle)
{
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Quaternion = DirectX::SimpleMath::Quaternion;

	// 位置・大きさ(1辺の半分)を設定
	m_rootPosition = position;
	m_bridgePosition = { 0.0f, HALF_LENGTH.y, 0.0f };
	m_halfLength = HALF_LENGTH;

	// 橋を倒す方向・軸を計算
	Vector3 rotAxis;
	CalculateBridgeAxis(angle, rotAxis);

	// モデルの読み込み
	m_model = pRM->RequestSDKMESH("bridge", "bridge.sdkmesh");

	// エフェクトの設定
	SetupEffect();

	// 当たり判定の設定
	SetupCollider(pCM, pEM);
	
	// アニメーションの設定
	SetupAnimation(rotAxis);
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void Bridge::Update(float elapsedTime)
{
	// アニメーションを再生中の場合
	if (m_tween->IsPlaying())
	{
		// アニメーションの更新
		m_tween->Update(elapsedTime, m_tweenParams);

		// 当たり判定を更新
		UpdateBridgeCollider();
	}

	// エフェクトの更新
	if (!m_isDowned)
	{
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
void Bridge::Draw(const RenderContext& context)
{
	using Matrix = DirectX::SimpleMath::Matrix;

	// ワールド行列の作成	
	Matrix world;
	Matrix rootTrans = Matrix::CreateTranslation(m_rootPosition);
	Matrix bridgeTrans = Matrix::CreateTranslation(m_bridgePosition);
	Matrix rot = Matrix::CreateFromQuaternion(m_tweenParams.rotation);
	world = bridgeTrans * rot * rootTrans;

	// モデルの描画
	m_model->Draw(context.deviceContext, *context.states, world, context.view, context.proj);

	// エフェクトの描画
	if (!m_isDowned)
	{
		m_effect->Draw(context);
	}

	// 当たり判定の描画(デバッグ用)
	//Matrix scale = Matrix::CreateScale(m_halfLength * 2.0f);
	//world = scale * bridgeTrans * rot * rootTrans;
	//m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Yellow, nullptr, true);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Bridge::Finalize()
{
	m_model = nullptr;
}



/**
 * @brief 橋を倒す方向・軸を計算
 *
 * @param angle		倒す角度 (Y軸)
 * @param outAxis	倒す軸の出力先
 *
 * @return なし
 */
void Bridge::CalculateBridgeAxis(float angle, DirectX::SimpleMath::Vector3& outAxis)
{
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Quaternion = DirectX::SimpleMath::Quaternion;

	// ラジアンに変換
	float radian = DirectX::XMConvertToRadians(angle);

	// モデルの向きを計算
	m_angle = Quaternion::CreateFromAxisAngle(Vector3::UnitY, radian);

	// 倒す方向を計算
	Vector3 dirAxis =
	{
		std::sinf(radian), 0.0f, std::cosf(radian)
	};
	if (dirAxis.LengthSquared() > 0.0f) dirAxis.Normalize();

	// 軸を計算 (倒す方向に垂直なベクトル)
	Vector3 rotAxis = Vector3::Up.Cross(dirAxis);
	if (rotAxis.LengthSquared() > 0.0f) rotAxis.Normalize();

	// 軸を出力先に代入
	outAxis = rotAxis;
}



/**
 * @brief 橋の当たり判定を更新
 *
 * @param なし
 *
 * @return なし
 */
void Bridge::UpdateBridgeCollider()
{
	// 回転後の橋の中心位置を計算
	DirectX::SimpleMath::Vector3 pos =
		DirectX::SimpleMath::Vector3::Transform(m_bridgePosition, m_tweenParams.rotation);

	// 当たり判定を更新
	m_collider.SetCenter(pos + m_rootPosition);
	m_collider.SetRotation(m_tweenParams.rotation);
}



/**
 * @brief エフェクトの設定
 *
 * @param なし
 *
 * @return なし
 */
void Bridge::SetupEffect()
{
	using Vector3 = DirectX::SimpleMath::Vector3;

	// エフェクトの大きさを計算
	Vector3 effectLength = Vector3::Transform(
		Vector3(m_halfLength.x * EFFECT_LENGTH_ADJUST, m_halfLength.y, m_halfLength.z * EFFECT_LENGTH_ADJUST), m_angle);

	// エフェクトの設定
	m_effect->SetPosition(Vector3(m_rootPosition.x, m_rootPosition.y, m_rootPosition.z));
	m_effect->SetLength(effectLength);
	m_effect->SetColor(DirectX::Colors::Yellow.v);
}



/**
 * @brief 当たり判定の設定
 *
 * @param pCM	当たり判定マネージャーのポインタ
 * @param pEM	敵マネージャーのポインタ
 *
 * @return なし
 */
void Bridge::SetupCollider(CollisionManager* pCM, EnemyManager* pEM)
{
	// 当たり判定の作成
	m_collider.SetCenter(m_rootPosition + m_bridgePosition);
	m_collider.SetHalfLength(m_halfLength);
	m_collider.SetRotation(m_angle);

	// 当たり判定マネージャーに登録
	CollisionManager::Desc desc{};
	desc.type = CollisionManager::Type::OBB;
	desc.layer = CollisionManager::Layer::Stage;
	desc.obb = &m_collider;
	desc.position = nullptr;
	desc.velocity = nullptr;
	desc.restitution = RESTITUTION;
	desc.callback.onEnter =
		[this, pCM, pEM](uint32_t, uint32_t other)
		{
			// 当たったのが敵本体なら通す
			if (pCM->GetDesc(other)->layer != CollisionManager::Layer::EnemyBody) return;

			// IDから敵を取得
			IEnemy* enemy = pEM->GetEnemyByID(pCM->GetDesc(other)->userId);

			// 当たった敵が跳ね返り状態なら橋を降ろす
			if (enemy->GetStateType() == StateType::Bounce && !m_isDowned)
			{
				m_isDowned = true;
				// アニメーションを再生
				m_tween->Play();
				// SEを再生
				OnMessageAccepted(Message::MessageID::SE_GIMMIC_SOLVE);
			}
		};
	m_collisionHandle = pCM->Add(desc);
}



/**
 * @brief アニメーションの設定
 *
 * @param rotAxis	倒す時の回転軸
 *
 * @return なし
 */
void Bridge::SetupAnimation(const DirectX::SimpleMath::Vector3& rotAxis)
{
	using Vector3 = DirectX::SimpleMath::Vector3;

	// 初期状態
	m_tweenParams =
	{
		m_rootPosition,
		Vector3::One,
		m_angle,
		1.0f
	};
	// 変化量
	Tween3D::UIParams delta =
	{
		Vector3::Zero,
		Vector3::Zero,
		DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(rotAxis, DirectX::XM_PIDIV2),
		0.0f
	};
	// アニメーションを設定
	Tween3D::TweenData tweenData =
	{
		m_tweenParams, delta, ANIMATION_TIME,
		Easing::EaseType::OutBounce, Easing::PlaybackMode::Once
	};
	m_tween = std::make_unique<Tween3D>(tweenData);
}
