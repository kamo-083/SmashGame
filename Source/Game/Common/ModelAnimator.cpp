/**
 * @file   ModelAnimator.cpp
 *
 * @brief  モデルアニメーターに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ModelAnimator.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param model		モデルのポインタ
 * @param animation アニメーションのポインタ
 */
ModelAnimator::ModelAnimator(DirectX::Model* model, DX::AnimationSDKMESH* animation)
	:
	m_model{ model },
	m_animation{ animation },
	m_animElapsedTime{ 0.0f },
	m_animEndTime{ 0.0f },
	m_loop{ false }

{

}


/**
 * @brief デストラクタ
 */
ModelAnimator::~ModelAnimator()
{
	m_model = nullptr;
	m_animation = nullptr;
}


/**
 * @brief 初期化処理
 *
 * @param endTime アニメーションの終了時間
 * @param loop	  アニメーションをループさせるか
 *
 * @return なし
 */
void ModelAnimator::Initialize(float endTime, bool loop)
{
	// アニメーションとモデルをバインドする
	m_animation->Bind(*m_model);

	// ボーン用のトランスフォーム配列を生成
	m_drawBones = DirectX::ModelBone::MakeArray(m_model->bones.size());

	// アニメーション時間をリセット
	m_animation->ResetTime();

	// 経過時間をリセット
	m_animElapsedTime = 0.0f;

	// 終了時間を設定
	m_animEndTime = endTime;

	// ループを設定
	m_loop = loop;
}


/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void ModelAnimator::Update(float elapsedTime)
{
	if (m_animElapsedTime >= m_animEndTime) return;

	m_animElapsedTime += elapsedTime;

	// アニメーションの更新
	m_animation->Update(elapsedTime);

	// ループ処理
	if (m_loop && m_animElapsedTime >= m_animEndTime)
	{
		m_animation->ResetTime();
		m_animElapsedTime = 0.0f;
	}
}


/**
 * @brief 描画処理
 *
 * @param context	 描画用構造体
 * @param world		 ワールド行列
 *
 * @return なし
 */
void ModelAnimator::Draw(const RenderContext& context, const DirectX::SimpleMath::Matrix& world)
{
	// ボーン数を取得する
	size_t nbones = m_model->bones.size();

	// アニメーションにモデルを適用する
	m_animation->Apply(*m_model, nbones, m_drawBones.get());

	// ライトの設定
	m_model->UpdateEffects([](DirectX::IEffect* effect)
		{
			if (auto lights = dynamic_cast<DirectX::IEffectLights*>(effect))
			{
				lights->SetAmbientLightColor(DirectX::Colors::White * AMBIENT_LIGHT_INTENSITY);

				// ライト0：前上から
				lights->SetLightEnabled(0, true);
				lights->SetLightDirection(0, DirectX::XMVector3Normalize(KEY_LIGHT.direction));
				lights->SetLightDiffuseColor(0, DirectX::Colors::White * KEY_LIGHT.intensity);

				// ライト1：左上から弱め
				lights->SetLightEnabled(1, true);
				lights->SetLightDirection(1, DirectX::XMVector3Normalize(FILL_LIGHT_LEFT.direction));
				lights->SetLightDiffuseColor(1, DirectX::Colors::White * FILL_LIGHT_LEFT.intensity);

				// ライト2：右上から弱め
				lights->SetLightEnabled(2, true);
				lights->SetLightDirection(2, DirectX::XMVector3Normalize(FILL_LIGHT_RIGHT.direction));
				lights->SetLightDiffuseColor(2, DirectX::Colors::White * FILL_LIGHT_RIGHT.intensity);

				// テカリ抑制
				lights->SetLightSpecularColor(0, DirectX::Colors::Black);
				lights->SetLightSpecularColor(1, DirectX::Colors::Black);
				lights->SetLightSpecularColor(2, DirectX::Colors::Black);
			}
		});

	// アニメーションモデルを描画する
	m_model->DrawSkinned(
		context.deviceContext,
		*context.states,
		nbones,
		m_drawBones.get(),
		world,
		context.view,
		context.proj
	);
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void ModelAnimator::Finalize()
{
	m_model = nullptr;
	m_animation = nullptr;
}
