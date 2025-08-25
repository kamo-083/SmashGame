#include "pch.h"
#include "WeaponUI.h"

using namespace DirectX;

WeaponUI::WeaponUI(float width, float height)
	:m_windowSize(width, height)
{
	m_weaponList.resize(static_cast<int>(WeaponType::TYPE_NUM));
	m_texture.resize(static_cast<int>(WeaponType::TYPE_NUM));
}

WeaponUI::~WeaponUI()
{
	m_texture.clear();
}

void WeaponUI::Initialize(ResourceManager* resourceManager, float width, float height)
{
	// 武器の設定
	for (int i = 0; i < static_cast<int>(WeaponType::TYPE_NUM); i++)
	{
		m_weaponList[i] = static_cast<WeaponType>(i);
	}

	// テクスチャの読み込み
	m_texture[static_cast<int>(WeaponType::BASIC)] = resourceManager->RequestTexture("weapon_basic", L"Resources/Textures/Weapon/bou.png");
	m_texture[static_cast<int>(WeaponType::ROLLING)] = resourceManager->RequestTexture("weapon_rolling", L"Resources/Textures/Weapon/glass_ball.png");
	m_texture[static_cast<int>(WeaponType::HEAVY)] = resourceManager->RequestTexture("weapon_heavy", L"Resources/Textures/Weapon/war_ishiono.png");

	// 画像サイズの設定
	m_textureSize = SimpleMath::Vector2(width, height);
}

void WeaponUI::Draw(DirectX::SpriteBatch* spriteBatch)
{
	SimpleMath::Vector2 displayPos = m_windowSize - m_textureSize;

	spriteBatch->Begin();

	for (int i = 0; i < static_cast<int>(WeaponType::TYPE_NUM); i++)
	{
		displayPos.x = m_windowSize.x - m_textureSize.x * std::abs(i - static_cast<int>(WeaponType::TYPE_NUM));
		if (i == 0)
		{
			spriteBatch->Draw(m_texture[static_cast<int>(m_weaponList[i])], SimpleMath::Vector2(displayPos.x, displayPos.y - 50.0f));
		}
		else
		{
			spriteBatch->Draw(m_texture[static_cast<int>(m_weaponList[i])], displayPos);
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
	
	bool plus = true;
	if (m_weaponList[0] > type)
	{
		plus = false;
	}

	for (WeaponType& l : m_weaponList)
	{
		if (plus) ++l;
		else	   --l;
	}
}
