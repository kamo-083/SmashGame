/**
 * @file   ModelAnimator.cpp
 *
 * @brief  ModelAnimatorに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ModelAnimator.h"

using namespace DirectX;
using namespace DX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
ModelAnimator::ModelAnimator(DirectX::Model* model, DX::AnimationSDKMESH* animation)
	: m_model{ model }
	, m_animation{ animation }
	, m_animElapsedTime{ 0.0f }
	, m_animEndTime{ 0.0f }
	, m_roop{ false }

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
 * @param[in] なし
 *
 * @return なし
 */
void ModelAnimator::Initialize(float endTime, bool roop)
{
	// アニメーションとモデルをバインドする
	m_animation->Bind(*m_model);

	// ボーン用のトランスフォーム配列を生成
	m_drawBones = ModelBone::MakeArray(m_model->bones.size());

	// アニメーション時間をリセット
	m_animation->ResetTime();

	// 終了時間を設定
	m_animEndTime = endTime;

	// ループを設定
	m_roop = roop;
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
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
	if (m_roop && m_animElapsedTime >= m_animEndTime)
	{
		m_animation->ResetTime();
		m_animElapsedTime = 0.0f;
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ModelAnimator::Draw(RenderContext context, const DirectX::SimpleMath::Matrix& world)
{
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

	//m_model->Draw(context.deviceContext, *context.states, world, context.view, context.projection);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ModelAnimator::Finalize()
{
	m_model = nullptr;
	m_animation = nullptr;
}
