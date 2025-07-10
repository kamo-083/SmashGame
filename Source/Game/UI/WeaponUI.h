#pragma once
#include"Source/Game/Data/WeaponData.h"
#include"Source/Game/Common/ResourceManager.h"

class WeaponUI
{
private:
	// 武器一覧
	std::vector<WeaponType> m_weaponList;

	// テクスチャ
	std::vector<ID3D11ShaderResourceView*> m_texture;

	// ウィンドウサイズ
	DirectX::SimpleMath::Vector2 m_windowSize;

	// 画像サイズ
	DirectX::SimpleMath::Vector2 m_textureSize;

public:
	// コンストラクタ・デストラクタ
	WeaponUI(float width, float height);
	~WeaponUI();

	// 初期化
	void Initialize(ResourceManager* resourceManager, float width = 100, float height = 100);

	// 描画
	void Draw(DirectX::SpriteBatch* spriteBatch);


	// 選択中の武器を設定
	void ChangeWeapon(WeaponType type, bool right);
	void ChangeWeapon(WeaponType type);
};

