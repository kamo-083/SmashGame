/**
 * @file   GuideArrow.h
 *
 * @brief  ガイド用矢印に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Data/TextureData.h"
#include "Source/Game/Common/DeviceResources.h"
#include "Source/Game/Common/RenderContext.h"

// クラスの宣言 ===============================================================
class BillboardSprite;
class ResourceManager;

// クラスの定義 ===============================================================
/**
 * @brief GuideArrow
 */
class GuideArrow
{
	// クラス定数の宣言 -------------------------------------------------
private:
	// 画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 TEXTURE_SIZE = { 286.0f, 350.0f };
	// 片道分の再生時間
	// 出現時
	static constexpr float SPAWN_ANIMATION_TIME = 1.0f;
	// 消滅時
	static constexpr float DESPAWN_ANIMATION_TIME = 0.5f;

	// データメンバの宣言 -----------------------------------------------
private:
	// ビルボードスプライト
	std::unique_ptr<BillboardSprite> m_billboardSprite;

	// テクスチャ情報
	TextureInfo m_textureInfo;

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GuideArrow(
		DX::DeviceResources* pDR,
		ResourceManager* pRM);

	// デストラクタ
	~GuideArrow();

// 操作
public:
	// 出現処理
	void Spawn(
		const DirectX::SimpleMath::Vector3& startPosition,
		const DirectX::SimpleMath::Vector3& endOffset);

	// 消滅処理
	void Despawn();

	// 更新処理
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& cameraPos,
		const DirectX::SimpleMath::Vector3& cameraUp);

	// 描画処理
	void Draw(const RenderContext& context);

// 取得/設定
public:

// 内部実装
private:

};
