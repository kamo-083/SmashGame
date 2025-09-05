#pragma once
#include"Source/Game/Data/WeaponData.h"
#include"Source/Game/Common/ResourceManager.h"

class WeaponUI
{
public:
	enum class Slide
	{
		NONE = -1,
		RIGHT,
		LEFT
	};

	static constexpr float SLIDE_DISTANCE = 5.0f;

private:
	// 武器一覧
	std::vector<WeaponType> m_weaponList;

	// テクスチャ
	std::vector<ID3D11ShaderResourceView*> m_textures;

	// ウィンドウサイズ
	DirectX::SimpleMath::Vector2 m_windowSize;

	// 画像サイズ
	DirectX::SimpleMath::Vector2 m_textureSize;

	// スライド状態
	Slide m_slide;

	// スライドの幅
	float m_slideWidth;

public:
	// コンストラクタ・デストラクタ
	WeaponUI(float width, float height);
	~WeaponUI();

	// 初期化
	void Initialize(ResourceManager* resourceManager, float width = 100, float height = 100);

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Draw(DirectX::SpriteBatch* spriteBatch);


	// 選択中の武器を設定
	void ChangeWeapon(WeaponType type, bool right);
	void ChangeWeapon(WeaponType type);
};

