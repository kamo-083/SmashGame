/**
 * @file   TitleScene.h
 *
 * @brief  テストシーンに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Scene.h"
#include "Source/Game/UI/UIWidget.h"


// クラスの宣言 ===============================================================


// クラスの定義 ===============================================================
/**
 * @brief テストシーン
 */
class TitleScene : public Scene
{
	// クラス定数の宣言 -------------------------------------------------
public:
	struct Textures
	{
		ID3D11ShaderResourceView* logo;
		ID3D11ShaderResourceView* pressSpaceKey;

		Textures()
			:logo(nullptr)
			,pressSpaceKey(nullptr)
		{}

		~Textures()
		{
			logo = nullptr;
			pressSpaceKey = nullptr;
		}
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// システム関連



	// オブジェクト関連
	std::unique_ptr<Textures> m_textures;

	std::unique_ptr<UIWidget> m_ui;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TitleScene(SceneManager* pSceneManager, UserResources* pUserResources);

	// デストラクタ
	~TitleScene();


	// 操作
public:
	// 初期化処理
	void Initialize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(RenderContext context, Imase::DebugFont* debugFont) override;

	// 終了処理
	void Finalize() override;


	// 取得/設定
public:


	// 内部実装
private:


};
