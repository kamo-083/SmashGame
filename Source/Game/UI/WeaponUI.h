#pragma once
#include"Source/Game/Data/WeaponData.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/UI/UIWidget.h"

class WeaponUI
{
public:
	enum class Direction
	{
		NONE = -1,
		RIGHT,
		LEFT
	};

	enum class Layout
	{
		LEFT,
		CENTER,
		RIGHT,

		DisplayNum
	};

	struct LayoutData
	{
		DirectX::SimpleMath::Vector2 pos;
		DirectX::SimpleMath::Vector2 scale;
		float opacity;
	};

	static constexpr float SLIDE_DISTANCE = 2.0f;

private:
	// 武器一覧
	std::vector<WeaponType> m_weaponList;

	// レイアウト
	std::vector<LayoutData> m_layoutList;

	// テクスチャ
	std::vector<ID3D11ShaderResourceView*> m_textures;

	// ウィンドウサイズ
	DirectX::SimpleMath::Vector2 m_windowSize;

	// 画像サイズ
	DirectX::SimpleMath::Vector2 m_textureSize;

	// ウィジェット
	std::vector<std::unique_ptr<UIWidget>> m_widgets;

	// スライド状態
	Direction m_lastDirection;

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
	void Draw(RenderContext context);

	// 終了
	void Finalize();


	// 選択中の武器を設定
	void ChangeWeapon(WeaponType type);

private:
	void Slide(Direction dir);
	void MakeParam(UIWidget& widget, const LayoutData& to);
};

