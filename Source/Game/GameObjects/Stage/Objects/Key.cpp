/**
 * @file   Key.cpp
 *
 * @brief  鍵に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Key.h"
#include "Source/Game/Common/ResourceManager.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pRM		リソースマネージャーのポインタ
 */
Key::Key(ID3D11DeviceContext* context, ResourceManager* pRM)
	: m_state(KeyState::NONE)
	, m_model(nullptr)
{
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);

	// モデルの読み込み
	m_model = pRM->RequestSDKMESH("key", "key.sdkmesh");
}



/**
 * @brief デストラクタ
 */
Key::~Key()
{

}



/**
 * @brief 出現処理
 *
 * @param spawnPos 出現位置
 * @param goalPos  ゴール位置
 *
 * @return なし
 */
void Key::Spawn(
	DirectX::SimpleMath::Vector3 spawnPos,
	DirectX::SimpleMath::Vector3 goalPos)
{
	// 使用済みならここで終了
	if (m_state != KeyState::NONE) return;

	// ゴールの位置を設定
	m_goalPos = goalPos;

	// アニメーションの設定
	SetupSpawnAnim(spawnPos);

	// アニメーションの開始
	m_tweenAnim->Play();

	// 状態の設定
	m_state = KeyState::SPAWN;
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void Key::Update(float elapsedTime)
{
	switch (m_state)
	{
	case Key::KeyState::SPAWN:		// 出現状態
		//アニメーションを更新
		m_tweenAnim->Update(elapsedTime, m_tweenParam);

		// アニメーションが終了していたら移動状態に移行
		if (m_tweenAnim->Finished())
		{
			// アニメーションを設定
			SetupFlyingAnim();
			m_tweenAnim->ResetTime();
			m_tweenAnim->Play();

			// 状態を変更
			m_state = KeyState::FLYING;
		}
		break;
	case Key::KeyState::FLYING:		// 移動状態
		// アニメーションを更新
		m_tweenAnim->Update(elapsedTime, m_tweenParam);

		// アニメーションが終了していたら移動済み状態に移行
		if (m_tweenAnim->Finished())
		{
			m_state = KeyState::FINISHED;
		}
		break;
	}
}



/**
 * @brief 描画処理
 *
 * @param context   描画用構造体
 * @param debugFont デバッグ用フォント
 *
 * @return なし
 */
void Key::Draw(RenderContext& context, DebugFont* debugFont)
{
	// 出現状態または移動状態なら描画
	if (m_state == KeyState::SPAWN || m_state==KeyState::FLYING)
	{
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_tweenParam.pos);
		DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_tweenParam.rotation);
		world = rot * trans;

		// モデルの描画
		m_model->Draw(context.deviceContext, *context.states, world, context.view, context.proj);

		// 当たり判定の描画(デバッグ用)
		//m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Yellow);
	}

	// デバッグ情報の追加
	debugFont->AddString(0, 300, DirectX::Colors::Yellow, L" state = %d", m_state);
	if (m_state == KeyState::SPAWN || m_state == KeyState::FLYING)
	{
		debugFont->AddString(0, 325, DirectX::Colors::Yellow, L"pos = %0.3f,%0.3f,%0.3f",
			m_tweenParam.pos.x, m_tweenParam.pos.y, m_tweenParam.pos.z);
		debugFont->AddString(0, 350, DirectX::Colors::Yellow, L"end = %0.3f,%0.3f,%0.3f",
			m_tweenAnim->GetTweenData().delta.pos.x, m_tweenAnim->GetTweenData().delta.pos.y, m_tweenAnim->GetTweenData().delta.pos.z);
		debugFont->AddString(0, 375, DirectX::Colors::Yellow, L"rot = %0.3f,%0.3f,%0.3f",
			m_tweenParam.rotation.x, m_tweenParam.rotation.y, m_tweenParam.rotation.z);
		debugFont->AddString(0, 400, DirectX::Colors::Yellow, L"end = %0.3f,%0.3f,%0.3f",
			m_tweenAnim->GetTweenData().delta.rotation.x, m_tweenAnim->GetTweenData().delta.rotation.y, m_tweenAnim->GetTweenData().delta.rotation.z);
	}
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Key::Finalize()
{
	m_model = nullptr;
}



/**
 * @brief 出現時のアニメーションを設定
 *
 * @param startPos 出現位置
 *
 * @return なし
 */
void Key::SetupSpawnAnim(DirectX::SimpleMath::Vector3 startPos)
{
	// 開始
	Tween3D::UIParams start = {
	startPos,
	DirectX::SimpleMath::Vector3(1.0f),
	DirectX::SimpleMath::Quaternion::Identity,
	1.0f
	};
	// 変化量
	Tween3D::UIParams delta = {
		DirectX::SimpleMath::Vector3(0.0f,SPAWN_HEIGHT,0.0f),
		DirectX::SimpleMath::Vector3(0.0f),
		DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
			DirectX::SimpleMath::Vector3::Up,DirectX::XM_PI),
		0.0f
	};
	// パラメータを設定
	Tween3D::TweenData data = {
		start,delta,ANIMATION_TIME,
		Easing::EaseType::OutQuart,
		Easing::PlaybackMode::Once
	};
	m_tweenAnim = std::make_unique<Tween3D>(data);
	m_tweenParam = start;
}



/**
 * @brief 移動時のアニメーションを設定
 *
 * @param なし
 *
 * @return なし
 */
void Key::SetupFlyingAnim()
{
	// 開始
	Tween3D::UIParams start = m_tweenParam;
	// 変化量
	Tween3D::UIParams delta = {
		m_goalPos - start.pos,
		DirectX::SimpleMath::Vector3(0.0f),
		DirectX::SimpleMath::Quaternion::Identity,
		0.0f
	};
	// パラメータを設定 
	Tween3D::TweenData data = {
		start,delta,ANIMATION_TIME,
		Easing::EaseType::OutQuart,
		Easing::PlaybackMode::Once
	};
	m_tweenAnim->SetTweenData(data);
}
