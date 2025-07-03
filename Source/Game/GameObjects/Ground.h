/**
 * @file   Ground.h
 *
 * @brief  地面に関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/Collision.h"
#include"Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
/**
 * @brief 地面
 */
class Ground
{
// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr DirectX::SimpleMath::Vector3 CENTER_POS = { 0.0f,-0.5f,0.0f };
	static constexpr DirectX::SimpleMath::Vector3 HALF_LENGTH = { 5.0f,0.5f,5.0f };
	static constexpr DirectX::SimpleMath::Vector3 ANGLE = { 0.0f,0.0f,0.0f };


// データメンバの宣言 -----------------------------------------------
private:
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_halfLength;
	DirectX::SimpleMath::Vector3 m_angle;

	OBBCollider m_collider;

	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Ground(ID3D11DeviceContext* context);

	// デストラクタ
	~Ground();


// 操作
public:
	// 初期化処理
	void Initialize(DirectX::SimpleMath::Vector3 position = CENTER_POS,
					DirectX::SimpleMath::Vector3 halfLength = HALF_LENGTH,
					DirectX::SimpleMath::Vector3 angle = ANGLE);

	// 更新処理
	void Update();

	// 描画処理
	void Draw(RenderContext& context);

	// 終了処理
	void Finalize();


// 取得/設定
public:
	OBBCollider GetCollider();


// 内部実装
private:


};
