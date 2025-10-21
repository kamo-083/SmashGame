/**
 * @file   OperationUI.h
 *
 * @brief  操作方法UIに関するヘッダファイル
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
	// テクスチャ群
	struct Textures
	{
		ID3D11ShaderResourceView* nomalArrow = nullptr;		// 通常矢印
		ID3D11ShaderResourceView* rotateArrow = nullptr;	// 回転矢印
		ID3D11ShaderResourceView* keyText = nullptr;		// 操作キー
		ID3D11ShaderResourceView* icon = nullptr;			// アイコン(必要なら)
	};

	// 表示レイアウト
	enum class Layout
	{
		CENTER,	// 中央 (無効状態で表示)
		LEFT,	// 左	(有効状態で表示)
		RIGHT,	// 右	(有効状態で表示)
		
		DisplayNum	// 表示数
	};

	// トゥイーン再生時間
	static constexpr float TWEEN_TIME = 0.25f;

	// テキスト画像を切り取る時の左端
	static constexpr float TEXT_UV_LEFT = 430.0f;

	// テキスト画像の1文字の大きさ
	static constexpr DirectX::SimpleMath::Vector2 TEXT_SIZE = { 50.0f,100.0f };

	// 矢印画像の大きさ
	static constexpr DirectX::SimpleMath::Vector2 ARROW_SIZE_DEFAULT = { 200.0f,100.0f };	// 通常
	static constexpr DirectX::SimpleMath::Vector2 ARROW_SIZE_ROTATE = { 200.0f,200.0f };	// 回転


	// データメンバの宣言 -----------------------------------------------
private:
	// ウィジェット
	std::vector<std::unique_ptr<UIWidget>> m_widgets;

	// テクスチャ群
	std::unique_ptr<Textures> m_textures;

	// 有効フラグ
	bool m_active;

	// アイコン表示位置
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
	// パラメータの切り替え
	void SwitchParam(bool active, UIWidget& widget);
};
