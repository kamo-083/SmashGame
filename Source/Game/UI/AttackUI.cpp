/**
 * @file   AttackUI.cpp
 *
 * @brief  AttackUIに関するソースファイル
 */

#include "pch.h"
#include "AttackUI.h"


AttackUI::AttackUI(float windowWidth, float windowHeight)
	: m_windowSize(windowWidth, windowHeight)
	, m_lastDirection{ Direction::NONE }
{
	m_attackList.resize(static_cast<int>(AttackType::TYPE_NUM));
	m_textures.resize(static_cast<int>(AttackType::TYPE_NUM));
}

AttackUI::~AttackUI()
{
	m_textures.clear();
}

void AttackUI::Initialize(ResourceManager* resourceManager, float texWidth, float texHeight)
{
	// 武器の設定
	for (int i = 0; i < static_cast<int>(AttackType::TYPE_NUM); i++)
	{
		m_attackList[i] = static_cast<AttackType>(i);
	}

	// テクスチャの読み込み
	m_textures[static_cast<int>(AttackType::BASIC)] = resourceManager->RequestPNG("attack_basic", L"Resources/Textures/UI/basicAtk.png");
	m_textures[static_cast<int>(AttackType::ROLLING)] = resourceManager->RequestPNG("attack_rolling", L"Resources/Textures/UI/rollingAtk.png");
	m_textures[static_cast<int>(AttackType::HEAVY)] = resourceManager->RequestPNG("attack_heavy", L"Resources/Textures/UI/heavyAtk.png");

	// 画像サイズの設定
	m_textureSize = DirectX::SimpleMath::Vector2(texWidth, texHeight);

	// スライド処理方向の初期化
	m_lastDirection = Direction::NONE;

	// 表示レイアウトの設定
	m_layoutList.resize(static_cast<int>(Layout::DisplayNum));

	DirectX::SimpleMath::Vector2 center = { m_windowSize.x - m_textureSize.x * 1.5f, m_windowSize.y - m_textureSize.y * 0.6f };
	float  offsetX = m_textureSize.x * 1.1f;

	m_layoutList[static_cast<int>(Layout::LEFT)] = { center + DirectX::SimpleMath::Vector2(-offsetX, 0), {0.8f,0.8f}, 0.6f };
	m_layoutList[static_cast<int>(Layout::CENTER)] = { center, {1.2f,1.2f}, 1.0f };
	m_layoutList[static_cast<int>(Layout::RIGHT)] = { center + DirectX::SimpleMath::Vector2(offsetX, 0), {0.8f,0.8f}, 0.6f };

	// ウィジェットの作成
	m_widgets.clear();
	for (int i = 0; i < static_cast<int>(Layout::DisplayNum); i++)
	{
		Tween::TweenData data{
			{ m_layoutList[i].pos, m_layoutList[i].scale, 0.0f, m_layoutList[i].opacity },
			{ {},{},0,0 },
			0.1f,
			Tween::Ease::OutQuart,
			Tween::PlaybackMode::Once
		};
		std::unique_ptr<UIWidget> widget = std::make_unique<UIWidget>();
		widget->Initialize(m_textures[i], data, m_textureSize, false);
		m_widgets.push_back(std::move(widget));
	}
	BindAttackSlots();

	// 操作方法UIの画像読み込み
	OperationUI::Textures uiTextures;
	uiTextures.nomalArrow = resourceManager->RequestPNG("arrow", L"Resources/Textures/UI/arrow_triangle.png");
	uiTextures.rotateArrow = resourceManager->RequestPNG("rotate", L"Resources/Textures/UI/arrow_rotate.png");
	uiTextures.keyText = resourceManager->RequestPNG("box", L"Resources/Textures/text/operationText.png");

	// 操作方法UIの作成
	m_operationUI = std::make_unique<OperationUI>();
	m_operationUI->Initialize(
		uiTextures,
		DirectX::SimpleMath::Vector2(center.x, center.y - m_textureSize.y * 0.6f),
		offsetX * 2.0f,
		true,
		DirectX::SimpleMath::Vector2(200.f, 135.f));
}

void AttackUI::Update(float elapsedTime)
{
	bool anyPlaying = false;
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

	m_operationUI->Update(elapsedTime);
}

void AttackUI::Draw(RenderContext context)
{
	context.spriteBatch->Begin(
		DirectX::SpriteSortMode_Deferred,
		context.states->NonPremultiplied(),
		context.states->LinearClamp()
	);

	// 各武器アイコンの描画
	for (auto& widget : m_widgets)
	{
		widget->Draw(context.spriteBatch);
	}

	// 操作方法UIの描画
	m_operationUI->Draw(context, false);

	context.spriteBatch->End();
}

void AttackUI::Finalize()
{
	m_layoutList.clear();

	for (auto& widget : m_widgets)
	{
		widget->Finalize();
	}
	m_widgets.clear();

	m_textures.clear();

	m_operationUI->Finalize();
	m_operationUI.reset();
}

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
	Tween::UIParams from = widget.GetParam();

	Tween::UIParams delta = {
		to.pos - from.pos,
		to.scale - from.scale,
		from.rotation,
		to.opacity - from.opacity
	};

	widget.SetParam(from, delta);
}

void AttackUI::BindAttackSlots()
{
	int N = static_cast<int>(AttackType::TYPE_NUM);
	int center = static_cast<int>(m_attackList[0]);
	int left = (center - 1 + N) % N;
	int right = (center + 1) % N;

	m_widgets[static_cast<int>(Layout::LEFT)]->SetTexture(m_textures[left]);
	m_widgets[static_cast<int>(Layout::CENTER)]->SetTexture(m_textures[center]);
	m_widgets[static_cast<int>(Layout::RIGHT)]->SetTexture(m_textures[right]);

	for (int i = 0; i < static_cast<int>(Layout::DisplayNum); ++i)
	{
		Tween::UIParams start{ m_layoutList[i].pos, m_layoutList[i].scale, 0.0f, m_layoutList[i].opacity };
		Tween::UIParams delta{ {},{},0,0 };
		m_widgets[i]->SetParam(start, delta);
		m_widgets[i]->GetTween()->ResetTime();
	}
}
