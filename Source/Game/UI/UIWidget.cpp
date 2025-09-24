/**
 * @file   UIWidget.cpp
 *
 * @brief  UIWidgetに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "UIWidget.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
UIWidget::UIWidget()
	: m_texture{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
UIWidget::~UIWidget()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void UIWidget::Initialize(ID3D11ShaderResourceView* texture,
						  const Tween::TweenData data, 
						  DirectX::SimpleMath::Vector2 size,
						  bool play)
{
	m_texture = texture;
	m_params = data.start;
	m_texSize = size;

	m_tween = std::make_unique<Tween>(data);

	if (play) m_tween->Play();
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void UIWidget::Update(float elapsedTime)
{
	m_tween->Update(elapsedTime, m_params);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void UIWidget::Draw(RenderContext context)
{
	SimpleMath::Color color = { 1, 1, 1, m_params.opacity };
	SimpleMath::Vector2 size = { m_texSize.x * 0.5f,m_texSize.y * 0.5f };

	context.spriteBatch->Begin(
		SpriteSortMode_Deferred,
		context.states->NonPremultiplied(),
		context.states->LinearClamp()
	);

	context.spriteBatch->Draw(m_texture, m_params.pos, nullptr, color, m_params.rotation, size, m_params.scale, SpriteEffects_None, 0.0f);

	context.spriteBatch->End();
}

/**
 * @brief 描画処理 (SpriteBatchのBegin/End無し)(設定画像以外を表示したい場合)
 *
 * @param[in] spriteBatch スプライトバッチのポインタ
 * @param[in] texture	  テクスチャのポインタ
 * @param[in] pos		  座標
 * @param[in] rect		  切り取り範囲
 * @param[in] rot		  回転
 * @param[in] size		  画像サイズ
 *
 * @return なし
 */
void UIWidget::Draw(DirectX::SpriteBatch* spriteBatch,
					ID3D11ShaderResourceView* texture,
					DirectX::SimpleMath::Vector2 pos,
					const RECT* rect,
					float rot)
{ 
	// 各引数が初期値だった場合、既に登録済みのデータを使う
	// テクスチャ
	if (!texture) texture = m_texture;

	// 描画サイズ
	SimpleMath::Vector2 srcSize = m_texSize;
	if (rect) 
	{
		srcSize.x = float(rect->right - rect->left);
		srcSize.y = float(rect->bottom - rect->top);
	}

	// 座標
	if (pos == SimpleMath::Vector2::Zero) pos = m_params.pos;
	else								  pos = m_params.pos + pos * m_params.scale;
	//else								  pos += m_params.pos;

	// 回転
	if (rot == FLT_MAX) rot = m_params.rotation;

	// 中心
	SimpleMath::Vector2 origin = srcSize * 0.5f;
	//if (size == SimpleMath::Vector2::Zero) origin = m_texSize * 0.5f;
	//else								   origin = size * 0.5f;

	// 透明度
	SimpleMath::Color color = { 1, 1, 1, m_params.opacity };

	spriteBatch->Draw(texture, pos, rect, color, rot, origin, m_params.scale);
}


/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void UIWidget::Finalize()
{
	m_texture = nullptr;

	m_tween->Finalize();
	m_tween.reset();
}

void UIWidget::TweenReset(bool play)
{
	m_params = m_tween->GetStartParams();
	m_tween->ResetTime();

	if (play) m_tween->Play();
}

void UIWidget::SetParam(Tween::UIParams start, Tween::UIParams delta)
{
	m_params = start;

	Tween::TweenData data = m_tween->GetTweenData();
	data.start = start;
	data.delta = delta;
	m_tween->SetTweenData(data);
}
