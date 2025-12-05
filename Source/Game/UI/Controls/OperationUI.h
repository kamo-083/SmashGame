/**
 * @file   OperationUI.h
 *
 * @brief  操作方法UIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/UserResources.h"
#include"Source/Game/UI/Elements/UIWidget.h"


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

	// 引数用構造体
	struct OperationUIDesc
	{
		Textures textures;											 // テクスチャ群
		DirectX::SimpleMath::Vector2 arrowNormalSize;				 // 通常矢印の画像サイズ
		DirectX::SimpleMath::Vector2 arrowRotateSize;				 // 回転矢印の画像サイズ
		DirectX::SimpleMath::Vector2 textSize;						 // テキスト画像の1文字分の大きさ
		DirectX::SimpleMath::Vector2 iconSize;						 // アイコン画像のサイズ(アイコン使用時)
		float textUVLeft;											 // テキスト画像の使用箇所の左端
		DirectX::SimpleMath::Vector2 arrowRotateAdjustPos = { 0,0 }; // 回転矢印の位置調整
		DirectX::SimpleMath::Vector2 iconAdjustPos = {0,0};			 // アイコン画像の位置調整
		float UIScale = 1.0f;										 // UI全体のスケール
	};

	// トゥイーン再生時間
	static constexpr float TWEEN_TIME = 0.25f;


	// データメンバの宣言 -----------------------------------------------
private:
	// ウィジェット
	std::vector<std::unique_ptr<UIWidget>> m_widgets;

	// テクスチャ群
	std::unique_ptr<Textures> m_textures;

	// 有効フラグ
	bool m_active;

	// UI全体のスケール
	float m_scale;

	// 通常矢印画像の大きさ
	DirectX::SimpleMath::Vector2 m_arrowSizeNormal;
	// 回転矢印画像の大きさ
	DirectX::SimpleMath::Vector2 m_arrowSizeRotate;

	// テキスト画像を切り取る時の左端
	float m_textUVLeft;
	// テキスト画像の1文字の大きさ
	DirectX::SimpleMath::Vector2 m_textSize;

	// アイコン表示位置
	DirectX::SimpleMath::Vector2 m_iconPos;
	// アイコン画像サイズ
	DirectX::SimpleMath::Vector2 m_iconSize;


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
		const OperationUIDesc& desc,
		DirectX::SimpleMath::Vector2 centerPos,
		float arrowInterval,
		bool active
	);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext context);

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
	// ウィジェットの初期設定
	void SetupWidgets(
		const OperationUIDesc& desc,
		const DirectX::SimpleMath::Vector2 centerPos,
		const float arrowInterval);

	// パラメータの切り替え
	void SwitchParam(bool active, UIWidget& widget);
};
