/**
 * @file   GuideArrow.cpp
 *
 * @brief  ガイド用矢印に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "GuideArrow.h"
#include "Source/Game/Effect/BillboardSprite.h"
#include "Source/Game/Common/ResourceManager.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pDR	デバイスリソースのポインタ
 * @param pRM	リソースマネージャーのポインタ
 */
GuideArrow::GuideArrow(
	DX::DeviceResources* pDR,
	ResourceManager* pRM)
{
	// ビルボードスプライトの作成
	m_billboardSprite = std::make_unique<BillboardSprite>(pDR);

	// テクスチャ情報を設定
	m_textureInfo =
	{
		pRM->RequestPNG("arrow_guide", "UI/arrow_guide.png"),
		TEXTURE_SIZE
	};
}

/**
 * @brief デストラクタ
 */
GuideArrow::~GuideArrow()
{

}

/**
 * @brief 出現処理
 *
 * @param startPosition	出現位置
 * @param endOffset		移動量
 *
 * @return なし
 */
void GuideArrow::Spawn(
	const DirectX::SimpleMath::Vector3& startPosition, 
	const DirectX::SimpleMath::Vector3& endOffset)
{
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Quaternion = DirectX::SimpleMath::Quaternion;

	Tween3D::TweenParams start =
	{
		{startPosition.x, startPosition.y, startPosition.z},
		Vector3::One,
		Quaternion::Identity,
		1.0f
	};
	Tween3D::TweenParams delta =
	{
		endOffset,
		Vector3::Zero,
		Quaternion::CreateFromAxisAngle(Vector3::UnitY, DirectX::XM_PIDIV2),
		0.0f
	};
	Tween3D::TweenData arrowTween =
	{
		start, delta, SPAWN_ANIMATION_TIME,
		Easing::EaseType::InQuart, Easing::PlaybackMode::PingPong
	};
	m_billboardSprite->Initialize(
		m_textureInfo.texture, arrowTween, m_textureInfo.size
	);
}

/**
 * @brief 消滅処理
 *
 * @param なし
 *
 * @return なし
 */
void GuideArrow::Despawn()
{
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Quaternion = DirectX::SimpleMath::Quaternion;

	// データの取得
	Tween3D::TweenParams nowParams = m_billboardSprite->GetParam();
	Tween3D::TweenParams deltaParams = m_billboardSprite->GetDelta();
	
	Tween3D::TweenParams start =
	{
		{nowParams.pos.x, nowParams.pos.y, nowParams.pos.z},
		Vector3::One,
		nowParams.rotation,
		1.0f
	};
	Tween3D::TweenParams delta =
	{
		deltaParams.pos,
		{-1.0f, -1.0f, 0.0f},
		Quaternion::CreateFromAxisAngle(Vector3::UnitY, DirectX::XM_PIDIV2),
		-1.0f
	};
	Tween3D::TweenData arrowTween =
	{
		start, delta, DESPAWN_ANIMATION_TIME,
		Easing::EaseType::OutQuart, Easing::PlaybackMode::Once
	};
	m_billboardSprite->Initialize(
		m_textureInfo.texture, arrowTween, m_textureInfo.size
	);
}

/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 * @param cameraPos		カメラ位置
 * @param cameraUp		カメラ上ベクトル
 *
 * @return なし
 */
void GuideArrow::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& cameraPos,
	const DirectX::SimpleMath::Vector3& cameraUp)
{
	if (m_billboardSprite->GetTween()->IsPlaying())
	{
		// 矢印の更新
		m_billboardSprite->Update(elapsedTime);
		// ビルボードを作成
		m_billboardSprite->CreateBillboard(cameraPos, cameraUp);
	}
}

/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void GuideArrow::Draw(const RenderContext& context)
{
	if (m_billboardSprite->GetTween()->IsPlaying())
	{
		// 矢印の描画
		m_billboardSprite->Draw(context);
	}
}