/**
 * @file   Key.cpp
 *
 * @brief  鍵に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Key.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param context	デバイスコンテキストのポインタ
 */
Key::Key(ID3D11DeviceContext* context)
	: m_state(KeyState::NONE)
{
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);

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
	// ゴールの位置を設定
	m_goalPos = goalPos;

	// アニメーションの設定
	SettingSpawnAnim(spawnPos);

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
	case Key::KeyState::SPAWN:
		//アニメーションを更新
		m_tweenAnim->Update(elapsedTime, m_tweenParam);

		// アニメーションが終了していたら移動状態に移行
		if (m_tweenAnim->Finished())
		{
			// アニメーションを設定
			SettingFlyingAnim();
			m_tweenAnim->ResetTime();
			m_tweenAnim->Play();

			// 状態を変更
			m_state = KeyState::FLYING;
		}
		break;
	case Key::KeyState::FLYING:
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
		m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Yellow);
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
	m_geometricPrimitive.reset();
}



/**
 * @brief 出現時のアニメーションを設定
 *
 * @param startPos 出現位置
 *
 * @return なし
 */
void Key::SettingSpawnAnim(DirectX::SimpleMath::Vector3 startPos)
{
	Tween3D::UIParams start = {
	startPos,
	DirectX::SimpleMath::Vector3(1.0f),
	DirectX::SimpleMath::Quaternion::Identity,
	1.0f
	};
	Tween3D::UIParams end = {
		DirectX::SimpleMath::Vector3(0.0f,2.0f,0.0f),
		DirectX::SimpleMath::Vector3(0.0f),
		DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
			DirectX::SimpleMath::Vector3::Up,DirectX::XMConvertToRadians(180.0f)),
		0.0f
	};
	Tween3D::TweenData data = {
		start,end,1.0f,
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
void Key::SettingFlyingAnim()
{
	Tween3D::UIParams start = m_tweenParam;
	Tween3D::UIParams end = {
		m_goalPos - start.pos,
		DirectX::SimpleMath::Vector3(0.0f),
		DirectX::SimpleMath::Quaternion::Identity,
		0.0f
	};
	Tween3D::TweenData data = {
		start,end,1.0f,
		Easing::EaseType::OutQuart,
		Easing::PlaybackMode::Once
	};

	m_tweenAnim->SetTweenData(data);
}
