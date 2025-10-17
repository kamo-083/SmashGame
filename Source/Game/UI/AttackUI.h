#pragma once
#include"Source/Game/Data/AttackData.h"
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/UI/UIWidget.h"
#include"Source/Game/UI/OperationUI.h"

class AttackUI
{
public:
	// スライド方向
	enum class Direction
	{
		NONE = -1,
		RIGHT,
		LEFT
	};

	// 表示レイアウト
	enum class Layout
	{
		LEFT,
		CENTER,
		RIGHT,

		DisplayNum	// 表示数
	};

	// 表示に必要な情報
	struct LayoutData
	{
		DirectX::SimpleMath::Vector2 pos;		// 表示位置
		DirectX::SimpleMath::Vector2 scale;		// 大きさ
		float opacity = 1.0f;					// 透明度
	};

	// スライド距離
	static constexpr float SLIDE_DISTANCE = 2.0f;

	// 基本の画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 DEFAULT_TEX_SIZE = { 200.f,200.f };


private:
	// 武器一覧
	std::vector<AttackType> m_attackList;

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

	// 操作方法UI
	std::unique_ptr<OperationUI> m_operationUI;


public:
	// コンストラクタ・デストラクタ
	AttackUI(float windowWidth, float windowHeight);
	~AttackUI();

	// 初期化
	void Initialize(ResourceManager* resourceManager, float texWidth = DEFAULT_TEX_SIZE.x, float texHeight = DEFAULT_TEX_SIZE.y);

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Draw(RenderContext context);

	// 終了
	void Finalize();

	// 選択中の武器を設定
	void ChangeAttack(AttackType type);

	// 操作方法UIの状態を切り替え
	void SwitchUIMode();

private:
	void Slide(Direction dir);
	void MakeParam(UIWidget& widget, const LayoutData& to);
	void BindAttackSlots();
};

