/**
 * @file   Sky.cpp
 *
 * @brief  Skyに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Sky.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
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
 * @param[in] なし
 *
 * @return なし
 */
void Sky::Initialize(UserResources* pUserResources)
{
	// スカイドームの読み込み
	m_skyDome = pUserResources->GetResourceManager()->RequestSDKMESH("skyDome", L"Resources\\Models\\skyDome.sdkmesh");
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Sky::Update(float elapsedTime)
{

}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Sky::Draw(const RenderContext& context)
{
	SimpleMath::Matrix world = SimpleMath::Matrix::Identity;
	if (m_pPosition) world *= SimpleMath::Matrix::CreateTranslation(*m_pPosition);

	m_skyDome->Draw(context.deviceContext, *context.states, world, context.view, context.proj);

	m_skyDome->UpdateEffects([](IEffect* effect)
		{
			auto lights = dynamic_cast<IEffectLights*>(effect);	// ライト
			if (lights)
			{
				lights->SetLightEnabled(0, false);
				lights->SetLightEnabled(1, false);
				lights->SetLightEnabled(2, false);
			}
			auto basicEffect = dynamic_cast<BasicEffect*>(effect);	// 自己発光
			if (basicEffect)
			{
				basicEffect->SetEmissiveColor(Colors::White);
			}
		}
	);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Sky::Finalize()
{
	m_skyDome = nullptr;
	m_pPosition = nullptr;
}
