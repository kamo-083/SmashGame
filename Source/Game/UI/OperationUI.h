/**
 * @file   OperationUI.h
 *
 * @brief  操作方法UIに関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/UserResources.h"
#include"Source/Game/UI/UIWidget.h"


// クラスの定義 ===============================================================
/**
 * @brief 操作方法UI
 */
class OperationUI
{
	// クラス定数の宣言 -------------------------------------------------
public:
	struct Textures
	{
		ID3D11ShaderResourceView* nomalArrow = nullptr;
		ID3D11ShaderResourceView* rotateArrow = nullptr;
		ID3D11ShaderResourceView* keyText = nullptr;
		ID3D11ShaderResourceView* icon = nullptr;
	};

	enum class Layout
	{
		CENTER,
		LEFT,
		RIGHT,

		DisplayNum
	};

	static constexpr float TWEEN_TIME = 0.25f;

	// テキスト画像を切り取る時の左端
	static constexpr float TEXT_UV_LEFT = 430.0f;

	// テキスト画像の1文字の大きさ
	static constexpr DirectX::SimpleMath::Vector2 TEXT_SIZE = { 50.0f,100.0f };


	// データメンバの宣言 -----------------------------------------------
private:
	std::vector<std::unique_ptr<UIWidget>> m_widgets;

	std::unique_ptr<Textures> m_textures;

	bool m_active;

	DirectX::SimpleMath::Vector2 m_iconPos;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	OperationUI();

	// デストラクタ
	~OperationUI();


	// 操作
public:
	// 初期化処理
	void Initialize(
		const Textures& textures,
		DirectX::SimpleMath::Vector2 centerPos,
		float arrowInterval,
		bool active,
		DirectX::SimpleMath::Vector2 iconSize = DirectX::SimpleMath::Vector2::Zero
	);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext context, bool batchBeginEnd = true);

	// 終了処理
	void Finalize();

	// 有効化・無効化
	void Active(bool active = true);


	// 取得/設定
public:
	// 有効かどうかを返す
	bool IsActive() { return m_active; }

	// 内部実装
private:
	void SwitchParam(bool active, UIWidget& widget);
};
