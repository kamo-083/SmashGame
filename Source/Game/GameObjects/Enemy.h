/**
 * @file   Enemy.h
 *
 * @brief  敵に関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Common/Collision.h"



// クラスの定義 ===============================================================
/**
 * @brief 敵
 */
class Enemy
{
// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr float RADIUS = 0.5f;	//半径の大きさ

	//リソース群
	struct Resources
	{
		DirectX::Model* model;
	};


// データメンバの宣言 -----------------------------------------------
private:
	//座標
	DirectX::SimpleMath::Vector3 m_position;

	//リソースマネージャ
	ResourceManager* m_pResourceManager;

	//リソース群
	Resources m_resources;

	//コライダー
	SphereCollider m_collider;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Enemy();

	// デストラクタ
	~Enemy();


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pResourceManager);

	// 更新処理
	void Update(DirectX::Keyboard::State keyboard, float elapsedTime);

	// 描画処理
	void Draw(RenderContext& context);

	// 終了処理
	void Finalize();


// 取得/設定
public:
	SphereCollider GetCollider();


// 内部実装
private:


};
