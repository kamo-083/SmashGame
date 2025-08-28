/**
 * @file   CountArea.h
 *
 * @brief  CountAreaに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include <functional>
#include"ImaseLib/DebugFont.h"
#include"Source/Game/Common/Collision.h"
#include"Source/Game/Common/CollisionManager.h"
#include"Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
/**
 * @brief CountArea
 */
class CountArea
{
	// クラス定数の宣言 -------------------------------------------------
public:
	static constexpr float AREA_HALF_HEIGHT = 1.0f;

	// 条件
	enum class TriggerMode
	{
		ReachCount,	// 目標数以上入れる
		AllOut		// 全部外に出す
	};

	// データメンバの宣言 -----------------------------------------------
private:
	// 座標
	DirectX::SimpleMath::Vector3 m_position;

	// モード
	TriggerMode m_mode;

	// 目標数
	int m_targetNum;

	// 目標に達しているか
	bool m_isTrigger;

	// 内部にいるか(AllOut用)
	bool m_armed;

	// エリア内にいる敵IDのリスト
	std::vector<uint32_t> m_insideList;

	// 条件を達成したときの動作
	std::function<void()> m_operation;

	// 当たり判定
	OBBCollider m_collider;

	// 衝突判定のハンドル
	uint32_t m_collisionHandle;

	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CountArea(ID3D11DeviceContext* context);

	// デストラクタ
	~CountArea();


// 操作
public:
	// 初期化処理
	void Initialize(CollisionManager* pCollisionManager,
					DirectX::SimpleMath::Vector3 position, float x, float z,
					std::function<void()> operation, TriggerMode mode, int targetNum = 0);

	// 更新処理
	void Update();

	// 描画処理
	void Draw(RenderContext& context, Imase::DebugFont* debugFont);

	// 終了処理
	void Finalize();

// 取得/設定
public:

// 内部実装
private:

};
