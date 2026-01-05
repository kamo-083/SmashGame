/**
 * @file   OperationUI.cpp
 *
 * @brief  操作方法UIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "OperationUI.h"
#include "Source/Game/Common/Keys/KeyAtlas.h"	


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
OperationUI::OperationUI()
	: m_active{ false }
	, m_scale{ 0.0f }
	, m_textWidth{ 0 }
{

};


/**
 * @brief デストラクタ
 */
OperationUI::~OperationUI()
{
	
}



/**
 * @brief 初期化処理
 *
 * @param desc			テクスチャ・サイズ等の情報
 * @param centerPos		中心位置
 * @param arrowInterval	矢印の間隔
 * @param active		有効フラグ
 *
 * @return なし
 */
void OperationUI::Initialize(
	const OperationUIDesc& desc,
	DirectX::SimpleMath::Vector2 centerPos,
	float arrowInterval,
	bool active)
{
	// 各サイズの設定
	m_arrowSizeNormal = desc.arrowNormalSize;	// 通常矢印
	m_arrowSizeRotate = desc.arrowRotateSize;	// 回転矢印
	m_textWidth = desc.textWidth;				// テキストの1文字分の幅
	m_iconSize = desc.iconSize;					// アイコン

	// 有効・無効の設定
	m_active = active;

	// 操作キーの設定
	m_inputKeys.clear();
	m_inputKeys.push_back(desc.keys.change);
	m_inputKeys.push_back(desc.keys.left);
	m_inputKeys.push_back(desc.keys.right);

	// UI全体のスケールの設定
	m_scale = desc.UIScale;

	// 画像の登録
	m_textures = std::make_unique<Textures>();
	m_textures->nomalArrow = desc.textures.nomalArrow;
	m_textures->rotateArrow = desc.textures.rotateArrow;
	m_textures->keyText = desc.textures.keyText;
	m_textures->icon = desc.textures.icon;

	// ウィジェットの作成
	SetupWidgets(desc, centerPos, arrowInterval);

	// アイコン位置の設定(アイコンが必要な場合)
	if (m_textures->icon)
	{
		m_iconPos = centerPos - (desc.iconSize * m_scale * 0.5f) + desc.iconAdjustPos;
	}
}


/**
 * @brief 更新処理
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void OperationUI::Update(float elapsedTime)
{
	// 各ウィジェットの更新
	for (auto& widget : m_widgets)
	{
		widget->Update(elapsedTime);
	}
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void OperationUI::Draw(RenderContext context)
{
	// 矢印を描画
	for (auto& widget : m_widgets)
	{
		widget->Draw(context.spriteBatch);
	}

	// 画像があればアイコンを描画
	if (m_textures->icon)
	{
		context.spriteBatch->Draw(
			m_textures->icon,
			m_iconPos,
			nullptr,
			DirectX::Colors::White,
			0.0f,
			DirectX::SimpleMath::Vector2::Zero,
			m_scale);
	}

	// 文字を描画
	int loopTime = 0;
	for (auto& widget : m_widgets)
	{
		RECT rect = KeyAtlas::GetRect(m_inputKeys[loopTime], m_textWidth);

		widget->Draw(
			context.spriteBatch,
			m_textures->keyText,
			DirectX::SimpleMath::Vector2::Zero,
			&rect,
			0.0f
		);

		loopTime++;
	}
}



/**
 * @brief 終了処理
 *
 * @param なし
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



/**
 * @brief 有効/無効化
 *
 * @param active 有効/無効
 *
 * @return なし
 */
void OperationUI::Active(bool active)
{
	if (m_active == active) return;

	m_active = active;

	// パラメータの切り替え
	ChangeParam(!m_active, *m_widgets[0].get());
	ChangeParam(m_active, *m_widgets[1].get());
	ChangeParam(m_active, *m_widgets[2].get());
}



/**
 * @brief ウィジェットの初期設定
 *
 * @param desc			テクスチャ・サイズ等の情報
 * @param centerPos		中心位置
 * @param arrowInterval	矢印の間隔
 *
 * @return なし
 */
void OperationUI::SetupWidgets(
	const OperationUIDesc& desc,
	const DirectX::SimpleMath::Vector2 centerPos, 
	const float arrowInterval)
{
	using vector2 = DirectX::SimpleMath::Vector2;

	// ウィジェットの作成
	m_widgets.resize(static_cast<int>(Layout::DisplayNum));

	// 中央(無効状態の時に表示する)
	std::unique_ptr<UIWidget> widget = std::make_unique<UIWidget>();
	Tween2D::TweenData data =
	{
		{centerPos + desc.arrowRotateAdjustPos,vector2::Zero,0.0f,1.0f},
		{vector2::Zero,vector2::Zero,0.0f,0.0f},
		TWEEN_TIME,
		Easing::EaseType::Linear,
		Easing::PlaybackMode::Once
	};
	widget->Initialize(m_textures->rotateArrow, data, m_arrowSizeRotate, false);
	ChangeParam(!m_active, *widget.get());
	m_widgets[static_cast<int>(Layout::CENTER)] = std::move(widget);

	// 左右(有効状態の時に表示する)
	float interval = arrowInterval * 0.5f * m_scale;
	// 左
	widget = std::make_unique<UIWidget>();
	vector2 leftPos = vector2(centerPos.x - interval, centerPos.y);
	data.start.pos = leftPos;
	data.start.rotation = DirectX::XM_PI;
	widget->Initialize(m_textures->nomalArrow, data, m_arrowSizeNormal, false);
	ChangeParam(m_active, *widget.get());
	m_widgets[static_cast<int>(Layout::LEFT)] = std::move(widget);

	// 右
	widget = std::make_unique<UIWidget>();
	vector2 rightPos = vector2(centerPos.x + interval, centerPos.y);
	data.start.pos = rightPos;
	data.start.rotation = 0.0f;
	widget->Initialize(m_textures->nomalArrow, data, m_arrowSizeNormal, false);
	ChangeParam(m_active, *widget.get());
	m_widgets[static_cast<int>(Layout::RIGHT)] = std::move(widget);
}



/**
 * @brief パラメータの切り替え
 *
 * @param active	有効フラグ
 * @param widget	変更するウィジェット
 *
 * @return なし
 */
void OperationUI::ChangeParam(bool active, UIWidget& widget)
{
	// 現在値で初期化した新規パラメータを作成
	Tween2D::TweenData newData = widget.GetTween()->GetTweenData();

	if (active)
	{
		// 出現させる
		newData.start.scale = DirectX::SimpleMath::Vector2(0.0f,0.0f);
		newData.start.opacity = 0.0f;
		newData.delta.scale = DirectX::SimpleMath::Vector2(m_scale);
		newData.delta.opacity = 1.0f;
		newData.ease = Easing::EaseType::OutBack;
	}
	else
	{
		// 退場させる
		newData.start.scale = DirectX::SimpleMath::Vector2(m_scale);
		newData.start.opacity = 1.0f;
		newData.delta.scale = DirectX::SimpleMath::Vector2(-m_scale);
		newData.delta.opacity = -1.0f;
		newData.ease = Easing::EaseType::OutQuart;
	}

	// アニメーションに設定
	widget.GetTween()->SetTweenData(newData);
	widget.TweenReset();
}