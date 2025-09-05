#include "pch.h"
#include "WeaponUI.h"

using namespace DirectX;

WeaponUI::WeaponUI(float width, float height)
	: m_windowSize(width, height)
	, m_slide{ Slide::NONE }
	, m_slideWidth{ 0.0f }
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

	// スライド処理関連の初期化
	m_slideWidth = 0.0f;
	m_slide = Slide::NONE;
}

void WeaponUI::Update(float elapsedTime)
{
	// スライド処理
	switch (m_slide)
	{
	case WeaponUI::Slide::NONE:
		return;
	case WeaponUI::Slide::RIGHT:
		m_slideWidth += SLIDE_DISTANCE;

		if (m_slideWidth >= 0.0f)
		{
			m_slideWidth = 0.0f;
			m_slide = Slide::NONE;
		}
		break;
	case WeaponUI::Slide::LEFT:
		m_slideWidth -= SLIDE_DISTANCE;

		if (m_slideWidth <= 0.0f)
		{
			m_slideWidth = 0.0f;
			m_slide = Slide::NONE;
		}
		break;
	}
}

void WeaponUI::Draw(DirectX::SpriteBatch* spriteBatch)
{
	SimpleMath::Vector2 displayPos = m_windowSize - m_textureSize;

	spriteBatch->Begin();

	for (int i = 0; i < static_cast<int>(WeaponType::TYPE_NUM); i++)
	{
		displayPos.x = m_windowSize.x + m_slideWidth - m_textureSize.x * std::abs(i - static_cast<int>(WeaponType::TYPE_NUM));
		if (i == 0)
		{
			spriteBatch->Draw(m_textures[static_cast<int>(m_weaponList[i])], SimpleMath::Vector2(displayPos.x, displayPos.y - 50.0f));
		}
		else
		{
			spriteBatch->Draw(m_textures[static_cast<int>(m_weaponList[i])], displayPos);
		}
	}

	spriteBatch->End();
}

void WeaponUI::ChangeWeapon(WeaponType type, bool right)
{
	if (m_weaponList[0] == type) return;

	for (WeaponType& l : m_weaponList)
	{
		if (right) ++l;
		else	   --l;
	}
}

void WeaponUI::ChangeWeapon(WeaponType type)
{
	if (m_weaponList[0] == type) return;

	// 切り替え前を保存
	WeaponType bf = *m_weaponList.begin();

	int typeNum = static_cast<int>(WeaponType::TYPE_NUM);
	int current = static_cast<int>(m_weaponList[0]);
	int target = static_cast<int>(type);

	// スライド距離を計算
	int right = (target - current + typeNum) % typeNum;
	int left = (current - target + typeNum) % typeNum;

	// スライド方向を設定
	bool goLeft = (right <= left);

	// スライド処理の有効化
	float width = m_textureSize.x * 0.5f;
	if (goLeft)
	{
		m_slide = Slide::LEFT;
		m_slideWidth = width;
	}
	else
	{
		m_slide = Slide::RIGHT;
		m_slideWidth = -width;
	}

	// 実データの回転
	for (WeaponType& l : m_weaponList)
	{
		if (goLeft) ++l;
		else		 --l;
	}
}
