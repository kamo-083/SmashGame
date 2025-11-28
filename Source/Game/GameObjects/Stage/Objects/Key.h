/**
 * @file   Key.h
 *
 * @brief  鍵に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Physics/Collision.h"
#include "Source/Game/Physics/CollisionManager.h"
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/GameObjects/Stage/Objects/Goal.h"


// クラスの定義 ===============================================================
/**
 * @brief 鍵
 */
class Key
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 鍵の状態
	enum class KeyState
	{
		NONE,		// 無し
		SPAWN,		// 出現
		FLYING,		// 移動中
		FINISHED	// 移動済み
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// ゴールの位置
	DirectX::SimpleMath::Vector3 m_goalPos;

	// 状態
	KeyState m_state;

	// 四角形
	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;

	// トゥイーン関連
	std::unique_ptr<Tween3D> m_tweenAnim;
	Tween3D::UIParams m_tweenParam;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Key(ID3D11DeviceContext* context);

	// デストラクタ
	~Key();


// 操作
public:
	// 出現処理
	void Spawn(
		DirectX::SimpleMath::Vector3 spawnPos,
		DirectX::SimpleMath::Vector3 goalPos);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext& context, DebugFont* debugFont);

	// 終了処理
	void Finalize();

// 取得/設定
public:
	// 状態の取得
	KeyState GetState() { return m_state; }

// 内部実装
private:
	// 出現時のアニメーションを設定
	void SettingSpawnAnim(DirectX::SimpleMath::Vector3 startPos);

	// 移動時のアニメーションを設定
	void SettingFlyingAnim();
};
