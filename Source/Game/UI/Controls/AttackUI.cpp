/**
 * @file   AttackUI.cpp
 *
 * @brief  攻撃方法UIに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "AttackUI.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/UI/Elements/UIWidget.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param windowWidth	ウィンドウの幅
 * @param windowHeight	ウィンドウの高さ
 */
AttackUI::AttackUI(float windowWidth, float windowHeight)
	: m_windowSize(windowWidth, windowHeight)
	, m_lastDirection{ Direction::NONE }
{
	m_attackList.resize(static_cast<int>(AttackType::TYPE_NUM));
	m_textures.resize(static_cast<int>(AttackType::TYPE_NUM));
}



/**
 * @brief デストラクタ
 */
AttackUI::~AttackUI()
{
	m_textures.clear();
}



/**
 * @brief 初期化処理
 *
 * @param attackDesc	攻撃方法UIの入力情報
 * @param operationDesc	操作方法UIの入力情報
 *
 * @return なし
 */
void AttackUI::Initialize(const AttackUIDesc& attackDesc, const OperationUI::OperationUIDesc& operationDesc)
{
	// 攻撃の設定
	for (int i = 0; i < static_cast<int>(AttackType::TYPE_NUM); i++)
	{
		m_attackList[i] = static_cast<AttackType>(i);
	}

	// テクスチャの読み込み
	m_textures[static_cast<int>(AttackType::BASIC)] = attackDesc.basicAtkTex;
	m_textures[static_cast<int>(AttackType::ROLLING)] = attackDesc.rollingAtkTex;
	m_textures[static_cast<int>(AttackType::HEAVY)] = attackDesc.heavyAtkTex;

	// 画像サイズの設定
	m_textureSize = DirectX::SimpleMath::Vector2(attackDesc.texWidth, attackDesc.texHeight);

	// スライド処理方向の初期化
	m_lastDirection = Direction::NONE;

	// 表示レイアウトの設定
	m_layoutList.resize(static_cast<int>(Layout::DisplayNum));

	DirectX::SimpleMath::Vector2 center = {
		m_textureSize.x * UI_POS_ADJUST_SCALE.x,
		m_windowSize.y - m_textureSize.y * UI_POS_ADJUST_SCALE.y };
	float  offsetX = m_textureSize.x * OFFSET_X_ADJUST_SCALE;

	m_layoutList[static_cast<int>(Layout::LEFT)] = { center + DirectX::SimpleMath::Vector2(-offsetX, 0), {0.8f,0.8f}, 0.6f };
	m_layoutList[static_cast<int>(Layout::CENTER)] = { center, {1.2f,1.2f}, 1.0f };
	m_layoutList[static_cast<int>(Layout::RIGHT)] = { center + DirectX::SimpleMath::Vector2(offsetX, 0), {0.8f,0.8f}, 0.6f };

	// ウィジェットの作成
	m_widgets.clear();
	for (int i = 0; i < static_cast<int>(Layout::DisplayNum); i++)
	{
		Tween2D::TweenData data{
			{ m_layoutList[i].pos, m_layoutList[i].scale, 0.0f, m_layoutList[i].opacity },
			{ {},{},0,0 },
			TWEEN_ANIM_TIME,
			Easing::EaseType::OutQuart,
			Easing::PlaybackMode::Once
		};
		std::unique_ptr<UIWidget> widget = std::make_unique<UIWidget>();
		widget->Initialize(m_textures[i], data, m_textureSize, false);
		m_widgets.push_back(std::move(widget));
	}
	BindAttackSlots();

	// 操作方法UIの作成
	m_operationUI = std::make_unique<OperationUI>();
	m_operationUI->Initialize(
		operationDesc,
		DirectX::SimpleMath::Vector2(center.x, center.y - m_textureSize.y * UI_POS_ADJUST_SCALE.y),
		offsetX * 2.0f,
		true
	);
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void AttackUI::Update(float elapsedTime)
{
	bool anyPlaying = false;

	// ウィジェットの更新
	for (auto& widget : m_widgets)
	{
		widget->Update(elapsedTime);
		anyPlaying |= !widget->GetTween()->Finished();
	}

	if (!anyPlaying && m_lastDirection != Direction::NONE)
	{
		BindAttackSlots();

		m_lastDirection = Direction::NONE;
	}

	// 操作方法UIの更新
	m_operationUI->Update(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param context		描画用構造体
 *
 * @return なし
 */
void AttackUI::Draw(RenderContext context)
{
	//context.spriteBatch->Begin(
	//	DirectX::SpriteSortMode_Deferred,
	//	context.states->NonPremultiplied(),
	//	context.states->LinearClamp()
	//);

	// 各攻撃アイコンの描画
	for (auto& widget : m_widgets)
	{
		widget->Draw(context.spriteBatch);
	}

	// 操作方法UIの描画
	m_operationUI->Draw(context);

	//context.spriteBatch->End();
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void AttackUI::Finalize()
{
	m_layoutList.clear();

	for (auto& widget : m_widgets)
	{
		widget->Finalize();
	}
	m_widgets.clear();

	for (auto& tex : m_textures)
	{
		tex = nullptr;
	}
	m_textures.clear();

	m_operationUI->Finalize();
	m_operationUI.reset();
}



/**
 * @brief 攻撃方法の変更
 *
 * @param type 変更先の攻撃方法
 *
 * @return なし
 */
void AttackUI::ChangeAttack(AttackType type)
{
	if (m_attackList[0] == type) return;

	int typeNum = static_cast<int>(AttackType::TYPE_NUM);
	int current = static_cast<int>(m_attackList[0]);
	int target = static_cast<int>(type);

	// スライド距離を計算
	int right = (target - current + typeNum) % typeNum;
	int left = (current - target + typeNum) % typeNum;

	// スライド方向を設定
	Direction dir = (right <= left) ? Direction::LEFT : Direction::RIGHT;

	// スライド処理の有効化
	Slide(dir);

	// 実データの回転
	for (AttackType& l : m_attackList)
	{
		if (dir == Direction::LEFT) ++l;
		else						--l;
	}
}

void AttackUI::SwitchUIMode()
{
	m_operationUI->Active(!m_operationUI->IsActive());
}

void AttackUI::Slide(Direction dir)
{
	// 最後にスライドした方向を記録
	if (m_lastDirection != dir)
	{
		m_lastDirection = dir;
	}

	for (int i = 0; i < static_cast<int>(Layout::DisplayNum); i++)
	{
		int j = 0;
		if (m_lastDirection == Direction::LEFT)
		{
			j = (i != 0) ? i - 1 : static_cast<int>(Layout::DisplayNum) - 1;
		}
		else if (m_lastDirection == Direction::RIGHT)
		{
			j = (i + 1 != static_cast<int>(Layout::DisplayNum)) ? i + 1 : 0;
		}

		MakeParam(*m_widgets[i], m_layoutList[j]);
	}

	for (auto& widget : m_widgets)
	{
		widget->TweenReset();
	}
}

void AttackUI::MakeParam(UIWidget& widget, const LayoutData& to)
{
	Tween2D::UIParams from = widget.GetParam();

	Tween2D::UIParams delta = {
		to.pos - from.pos,
		to.scale - from.scale,
		from.rotation,
		to.opacity - from.opacity
	};

	widget.SetParam(from, delta);
}

void AttackUI::BindAttackSlots()
{
	// 攻撃種類数
	int N = static_cast<int>(AttackType::TYPE_NUM);

	// リストの先頭の攻撃方法を中央のスロットに設定
	int center = static_cast<int>(m_attackList[0]);
	// 中央の攻撃方法を元に左右も設定
	int left = (center - 1 + N) % N;
	int right = (center + 1) % N;

	// 攻撃の種類に合ったアイコンを設定
	m_widgets[static_cast<int>(Layout::LEFT)]->SetTexture(m_textures[left]);
	m_widgets[static_cast<int>(Layout::CENTER)]->SetTexture(m_textures[center]);
	m_widgets[static_cast<int>(Layout::RIGHT)]->SetTexture(m_textures[right]);

	// 各ウィジェットのパラメータをリセット
	for (int i = 0; i < static_cast<int>(Layout::DisplayNum); ++i)
	{
		Tween2D::UIParams start{ m_layoutList[i].pos, m_layoutList[i].scale, 0.0f, m_layoutList[i].opacity };
		Tween2D::UIParams delta{ {},{},0,0 };
		m_widgets[i]->SetParam(start, delta);
		m_widgets[i]->GetTween()->ResetTime();
	}
}
