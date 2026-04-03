/**
 * @file   AttackUI.h
 *
 * @brief  攻撃方法UIに関するヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Data/AttackData.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/UI/Controls/OperationUI.h"
#include"Source/Game/Object/AudioListener.h"

// クラスの宣言 ===============================================================
class UIWidget;
class ResourceManager;

// クラスの定義 ===============================================================
/**
 * @brief 攻撃方法UI
 */
class AttackUI
{
// クラス定数の宣言 -------------------------------------------------
public:
	// 引数用構造体
	struct AttackUIDesc
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> basicAtkTex;	// 通常攻撃画像
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rollingAtkTex;	// 転がり攻撃画像
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> heavyAtkTex;	// 強攻撃画像
		float texWidth;		// 画像の幅
		float texHeight;	// 画像の高さ
	};

	// スライド方向
	enum class Direction
	{
		NONE = -1,
		RIGHT,		// 右
		LEFT		// 左
	};

	// 表示レイアウト
	enum class Layout
	{
		LEFT,	// 左
		CENTER,	// 中央
		RIGHT,	// 右

		DisplayNum	// 表示数
	};

	// 表示に必要な情報
	struct LayoutData
	{
		DirectX::SimpleMath::Vector2 pos;		// 表示位置
		DirectX::SimpleMath::Vector2 scale;		// 大きさ
		float opacity = 1.0f;					// 透明度
	};

	// デフォルトの画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 DEFAULT_TEX_SIZE = { 100.f,100.f };

	static constexpr float TWEEN_ANIM_TIME = 0.1f;	// トゥイーンアニメーションの時間

	// 調整用倍率
	static constexpr DirectX::SimpleMath::Vector2 UI_POS_ADJUST_SCALE = { 1.5f,0.6f };	// 中心位置の調整
	static constexpr float OFFSET_X_ADJUST_SCALE = 1.1f;	// 中心位置の調整

// データメンバの宣言 -----------------------------------------------
private:
	// 攻撃方法リスト
	std::vector<AttackType> m_attackList;

	// レイアウト
	std::vector<LayoutData> m_layoutList;

	// テクスチャ
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textures;

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

	// 音声再生機能
	AudioListener m_audio;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	AttackUI(float windowWidth, float windowHeight, AudioManager* pAM);

	// デストラクタ
	~AttackUI();

// 操作
	// 初期化
	void Initialize(const AttackUIDesc& attackDesc, const OperationUI::OperationUIDesc& operationDesc);

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Draw(const RenderContext& context);

	// 終了
	void Finalize();

	// 選択中の攻撃を設定
	void ChangeAttack(AttackType type);

	// 操作方法UIの状態を切り替え
	void SwitchUIMode();

// 取得/設定
public:

// 内部実装
private:
	// スライド処理
	void Slide(Direction dir);
	
	// パラメータの作成
	void MakeParam(UIWidget& widget, const LayoutData& to);

	// 攻撃リストとUI画像を一致させる
	void BindAttackSlots();
};