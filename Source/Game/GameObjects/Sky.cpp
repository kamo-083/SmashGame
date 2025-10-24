/**
 * @file   Sky.cpp
 *
 * @brief  スカイドームに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Sky.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
Sky::Sky()
	: m_skyDome{ nullptr }
	, m_pPosition{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
Sky::~Sky()
{

}



/**
 * @brief 初期化処理
 *
 * @param pUR	ユーザーリソースのポインタ
 *
 * @return なし
 */
void Sky::Initialize(UserResources* pUR)
{
	// スカイドームの読み込み
	m_skyDome = pUR->GetResourceManager()->RequestSDKMESH("skyDome", L"Resources\\Models\\skyDome.sdkmesh");
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void Sky::Draw(const RenderContext& context)
{
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;

	// 中心座標のポインタが設定されていた場合は反映
	if (m_pPosition) world *= DirectX::SimpleMath::Matrix::CreateTranslation(*m_pPosition);

	m_skyDome->Draw(context.deviceContext, *context.states, world, context.view, context.proj);

	m_skyDome->UpdateEffects([](DirectX::IEffect* effect)
		{
			auto lights = dynamic_cast<DirectX::IEffectLights*>(effect);	// ライト
			if (lights)
			{
				lights->SetLightEnabled(0, false);
				lights->SetLightEnabled(1, false);
				lights->SetLightEnabled(2, false);
			}
			auto basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);	// 自己発光
			if (basicEffect)
			{
				basicEffect->SetEmissiveColor(DirectX::Colors::White);
			}
		}
	);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Sky::Finalize()
{
	m_skyDome = nullptr;
	m_pPosition = nullptr;
}
