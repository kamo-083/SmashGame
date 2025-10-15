/**
 * @file   OperationUI.cpp
 *
 * @brief  操作方法UIに関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "OperationUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
OperationUI::OperationUI()
	: m_active{ false }
{

}



/**
 * @brief デストラクタ
 */
OperationUI::~OperationUI()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void OperationUI::Initialize(const Textures& textures, 
							 DirectX::SimpleMath::Vector2 centerPos, 
							 float arrowInterval,
							 bool active,
							 DirectX::SimpleMath::Vector2 iconSize)
{
	// 有効・無効の設定
	m_active = active;

	// 画像の登録
	m_textures = std::make_unique<Textures>();
	m_textures->nomalArrow = textures.nomalArrow;
	m_textures->rotateArrow = textures.rotateArrow;
	m_textures->keyText = textures.keyText;
	m_textures->icon = textures.icon;

	// ウィジェットの作成
	m_widgets.resize(static_cast<int>(Layout::DisplayNum));

	// 中央(無効状態の時に表示する)
	std::unique_ptr<UIWidget> widget = std::make_unique<UIWidget>();
	Tween::TweenData data =
	{
		{centerPos,DirectX::SimpleMath::Vector2::Zero,0.0f,1.0f},
		{DirectX::SimpleMath::Vector2::Zero,DirectX::SimpleMath::Vector2::Zero,0.0f,0.0f},
		TWEEN_TIME,
		Tween::Ease::Liner,
		Tween::PlaybackMode::Once
	};
	widget->Initialize(m_textures->rotateArrow, data, DirectX::SimpleMath::Vector2(200.f, 200.f), false);
	SwitchParam(!m_active, *widget.get());
	m_widgets[static_cast<int>(Layout::CENTER)] = std::move(widget);

	// 左右(有効状態の時に表示する)
	// 左
	widget = std::make_unique<UIWidget>();
	DirectX::SimpleMath::Vector2 leftPos = DirectX::SimpleMath::Vector2(centerPos.x - arrowInterval * 0.5f, centerPos.y);
	data.start.pos = leftPos;
	data.start.rotation = DirectX::XM_PI;
	widget->Initialize(m_textures->nomalArrow, data, DirectX::SimpleMath::Vector2(200.f, 100.f), false);
	SwitchParam(m_active, *widget.get());
	m_widgets[static_cast<int>(Layout::LEFT)] = std::move(widget);

	// 右
	widget = std::make_unique<UIWidget>();
	DirectX::SimpleMath::Vector2 rightPos = DirectX::SimpleMath::Vector2(centerPos.x + arrowInterval * 0.5f, centerPos.y);
	data.start.pos = rightPos;
	data.start.rotation = 0.0f;
	widget->Initialize(m_textures->nomalArrow, data, DirectX::SimpleMath::Vector2(200.f, 100.f), false);
	SwitchParam(m_active, *widget.get());
	m_widgets[static_cast<int>(Layout::RIGHT)] = std::move(widget);

	// アイコン座標の設定
	if (m_textures->icon)
	{
		m_iconPos = m_widgets[static_cast<int>(Layout::CENTER)]->GetTween()->GetStartParams().pos - iconSize * 0.5f;
	}
}


/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void OperationUI::Update(float elapsedTime)
{
	for (auto& widget : m_widgets)
	{
		widget->Update(elapsedTime);
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void OperationUI::Draw(RenderContext context, bool batchBeginEnd)
{
	if (batchBeginEnd)
	{
		context.spriteBatch->Begin(
			DirectX::SpriteSortMode_Deferred,
			context.states->NonPremultiplied(),
			context.states->LinearClamp()
		);
	}

	// 矢印を描画
	for (auto& widget : m_widgets)
	{
		widget->Draw(context.spriteBatch);
	}

	// 画像があればアイコンを描画
	if (m_textures->icon)
	{
		context.spriteBatch->Draw(m_textures->icon, m_iconPos);
	}

	// 文字を描画
	int loopTime = 0;
	for (auto& widget : m_widgets)
	{
		float uvLeft = TEXT_UV_LEFT + TEXT_SIZE.x * loopTime;
		RECT rect = { uvLeft, 0.0f, uvLeft + TEXT_SIZE.x, TEXT_SIZE.y };

		widget->Draw(
			context.spriteBatch,
			m_textures->keyText,
			DirectX::SimpleMath::Vector2::Zero,
			&rect,
			0.0f
		);

		loopTime++;
	}

	if (batchBeginEnd) context.spriteBatch->End();
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void OperationUI::Finalize()
{
	m_textures.reset();

	for (auto& widget : m_widgets)
	{
		widget->Finalize();
	}
	m_widgets.clear();
}


void OperationUI::Active(bool active)
{
	if (m_active == active) return;

	m_active = active;

	SwitchParam(!m_active, *m_widgets[0].get());
	SwitchParam(m_active, *m_widgets[1].get());
	SwitchParam(m_active, *m_widgets[2].get());
}

void OperationUI::SwitchParam(bool active, UIWidget& widget)
{
	Tween::TweenData nowData = widget.GetTween()->GetTweenData();
	Tween::TweenData newData = nowData;

	if (active)
	{
		newData.start.scale = DirectX::SimpleMath::Vector2(0.0f,0.0f);
		newData.start.opacity = 0.0f;
		newData.delta.scale = DirectX::SimpleMath::Vector2(1.0f,1.0f);
		newData.delta.opacity = 1.0f;
		newData.ease = Tween::Ease::OutBack;
	}
	else
	{
		newData.start.scale = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
		newData.start.opacity = 1.0f;
		newData.delta.scale = DirectX::SimpleMath::Vector2(-1.0f, -1.0f);
		newData.delta.opacity = -1.0f;
		newData.ease = Tween::Ease::OutQuart;
	}

	widget.GetTween()->SetTweenData(newData);
	widget.TweenReset();
}