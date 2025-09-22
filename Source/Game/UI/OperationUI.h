/**
 * @file   OperationUI.h
 *
 * @brief  操作方法UIに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
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
		ID3D11ShaderResourceView* arrow;
		ID3D11ShaderResourceView* box;
		ID3D11ShaderResourceView* keyText;
	};

	static constexpr float TWEEN_TIME = 0.25f;


	// データメンバの宣言 -----------------------------------------------
private:
	std::vector<std::unique_ptr<UIWidget>> m_widgets;

	std::unique_ptr<Textures> m_textures;

	bool m_active;


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
		ResourceManager* resourceManager,
		DirectX::SimpleMath::Vector2 centerPos,
		float arrowInterval,
		bool active);

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

	// 内部実装
private:
	void SwitchParam(bool active, UIWidget& widget);
};
