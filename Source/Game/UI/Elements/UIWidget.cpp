/**
 * @file   UIWidget.cpp
 *
 * @brief  UIウィジェット(アニメーション付きUIのベース)に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "UIWidget.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
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
 * @param texture	テクスチャのポインタ
 * @param data		トゥイーン情報
 * @param size		テクスチャのサイズ
 * @param play		再生するか
 *
 * @return なし
 */
void UIWidget::Initialize(ID3D11ShaderResourceView* texture,
						  const Tween2D::TweenData data,
						  DirectX::SimpleMath::Vector2 size,
						  bool play)
{
	// テクスチャ・アニメーションの設定
	m_texture = texture;
	m_params = data.start;
	m_texSize = size;

	m_tween = std::make_unique<Tween2D>(data);

	// トゥイーンの再生
	if (play) m_tween->Play();
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void UIWidget::Update(float elapsedTime)
{
	// パラメータの更新
	m_tween->Update(elapsedTime, m_params);
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void UIWidget::Draw(RenderContext context)
{
	// 透明度・画像サイズの設定
	DirectX::SimpleMath::Color color = { 1, 1, 1, m_params.opacity };
	DirectX::SimpleMath::Vector2 size = { m_texSize.x * 0.5f, m_texSize.y * 0.5f };

	context.spriteBatch->Begin(
		DirectX::SpriteSortMode_Deferred,
		context.states->NonPremultiplied(),
		context.states->LinearClamp()
	);

	context.spriteBatch->Draw(m_texture, m_params.pos, nullptr, color, m_params.rotation, size, m_params.scale, DirectX::SpriteEffects_None, 0.0f);

	context.spriteBatch->End();
}

/**
 * @brief 描画処理 (SpriteBatchのBegin/End無し)
 *
 * @param spriteBatch スプライトバッチのポインタ(設定画像以外を表示したい場合)
 * @param texture	  テクスチャのポインタ
 * @param pos		  座標
 * @param rect		  切り取り範囲
 * @param rot		  回転(ラジアン)
 * @param size		  画像サイズ
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
	DirectX::SimpleMath::Vector2 srcSize = m_texSize;
	if (rect) 
	{
		srcSize.x = float(rect->right - rect->left);
		srcSize.y = float(rect->bottom - rect->top);
	}

	// 座標
	if (pos == DirectX::SimpleMath::Vector2::Zero) pos = m_params.pos;
	else										   pos = m_params.pos + pos * m_params.scale;

	// 回転
	if (rot == FLT_MAX) rot = m_params.rotation;

	// 中心
	DirectX::SimpleMath::Vector2 origin = srcSize * 0.5f;

	// 透明度
	DirectX::SimpleMath::Color color = { 1, 1, 1, m_params.opacity };

	spriteBatch->Draw(texture, pos, rect, color, rot, origin, m_params.scale);
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void UIWidget::Finalize()
{
	m_texture = nullptr;

	m_tween->Finalize();
	m_tween.reset();
}


/**
 * @brief トゥイーンのリセット
 *
 * @param play 再生するか
 *
 * @return なし
 */
void UIWidget::TweenReset(bool play)
{
	// パラメータ・アニメーションの初期化
	m_params = m_tween->GetStartParams();
	m_tween->ResetTime();
	m_tween->ResetPlayed();

	// すぐに再生するか
	if (play) m_tween->Play();
	else	  m_tween->End();
}


/**
 * @brief 新しいパラメータの設定
 *
 * @param start	開始時のパラメータ
 * @param delta	パラメータの変化量
 *
 * @return なし
 */
void UIWidget::SetParam(Tween2D::UIParams start, Tween2D::UIParams delta)
{
	// 現在のパラメータを更新
	m_params = start;

	// アニメーションにパラメータを設定
	Tween2D::TweenData data = m_tween->GetTweenData();
	data.start = start;
	data.delta = delta;
	m_tween->SetTweenData(data);
}
