#include "pch.h"
#include "WeaponUI.h"

using namespace DirectX;

WeaponUI::WeaponUI(float width, float height)
	: m_windowSize(width, height)
	, m_lastDirection{ Direction::NONE }
{
	m_weaponList.resize(static_cast<int>(WeaponType::TYPE_NUM));
	m_textures.resize(static_cast<int>(WeaponType::TYPE_NUM));
}

WeaponUI::~WeaponUI()
{
	m_textures.clear();
}

void WeaponUI::Initialize(ResourceManager* resourceManager, float width, float height)
{
	// 武器の設定
	for (int i = 0; i < static_cast<int>(WeaponType::TYPE_NUM); i++)
	{
		m_weaponList[i] = static_cast<WeaponType>(i);
	}

	// テクスチャの読み込み
	m_textures[static_cast<int>(WeaponType::BASIC)] = resourceManager->RequestTexture("weapon_basic", L"Resources/Textures/Weapon/bou.png");
	m_textures[static_cast<int>(WeaponType::ROLLING)] = resourceManager->RequestTexture("weapon_rolling", L"Resources/Textures/Weapon/glass_ball.png");
	m_textures[static_cast<int>(WeaponType::HEAVY)] = resourceManager->RequestTexture("weapon_heavy", L"Resources/Textures/Weapon/war_ishiono.png");

	// 画像サイズの設定
	m_textureSize = SimpleMath::Vector2(width, height);

	// スライド処理方向の初期化
	m_lastDirection = Direction::NONE;

	// 表示レイアウトの設定
	m_layoutList.resize(static_cast<int>(Layout::DisplayNum));

	SimpleMath::Vector2 center = { m_windowSize.x - m_textureSize.x*2.0f, m_windowSize.y - m_textureSize.y * 0.8f };
	float  offsetX = m_textureSize.x * 1.2f; 
	
	m_layoutList[static_cast<int>(Layout::LEFT)] = { center + SimpleMath::Vector2(-offsetX, 0), {0.8f,0.8f}, 0.6f };
	m_layoutList[static_cast<int>(Layout::CENTER)] = { center, {1.2f,1.2f}, 1.0f };
	m_layoutList[static_cast<int>(Layout::RIGHT)] = { center + SimpleMath::Vector2(+offsetX, 0), {0.8f,0.8f}, 0.6f };

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
	BindWeaponSlots();
}

void WeaponUI::Update(float elapsedTime)
{
	bool anyPlaying = false;
	for (auto& widget : m_widgets)
	{
		widget->Update(elapsedTime);
		anyPlaying |= !widget->GetTween()->Finished();
	}

	if (!anyPlaying && m_lastDirection != Direction::NONE)
	{
		BindWeaponSlots();

		m_lastDirection = Direction::NONE;
	}
}

void WeaponUI::Draw(RenderContext context)
{
	context.spriteBatch->Begin(
		SpriteSortMode_Deferred,
		context.states->NonPremultiplied(),
		context.states->LinearClamp()
	);

	for (auto& widget : m_widgets)
	{
		widget->Draw(context.spriteBatch);
	}

	context.spriteBatch->End();
}

void WeaponUI::Finalize()
{
	m_layoutList.clear();

	for (auto& widget : m_widgets)
	{
		widget->Finalize();
	}
	m_widgets.clear();

	m_textures.clear();
}

void WeaponUI::ChangeWeapon(WeaponType type)
{
	if (m_weaponList[0] == type) return;

	int typeNum = static_cast<int>(WeaponType::TYPE_NUM);
	int current = static_cast<int>(m_weaponList[0]);
	int target = static_cast<int>(type);

	// スライド距離を計算
	int right = (target - current + typeNum) % typeNum;
	int left = (current - target + typeNum) % typeNum;

	// スライド方向を設定
	Direction dir = (right <= left) ? Direction::LEFT : Direction::RIGHT;

	// スライド処理の有効化
	Slide(dir);

	// 実データの回転
	for (WeaponType& l : m_weaponList)
	{
		if (dir == Direction::LEFT) ++l;
		else						--l;
	}
}

void WeaponUI::Slide(Direction dir)
{
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

void WeaponUI::MakeParam(UIWidget& widget, const LayoutData& to)
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

void WeaponUI::BindWeaponSlots()
{
	int N = static_cast<int>(WeaponType::TYPE_NUM);
	int center = static_cast<int>(m_weaponList[0]);
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
