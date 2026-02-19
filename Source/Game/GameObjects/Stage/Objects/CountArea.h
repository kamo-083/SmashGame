/**
 * @file   CountArea.h
 *
 * @brief  エリアに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include <functional>
#include"Source/Debug/DebugFont.h"
#include"Source/Game/Common/UserResources.h"
#include"Source/Game/Physics/Collision.h"
#include"Source/Game/Physics/CollisionManager.h"
#include"Source/Game/Common/ShaderManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/UI/Elements/NumberRenderer/NumberRenderer3D.h"

// クラスの宣言 ===============================================================
class ResourceManager;
class AreaEffect;


// クラスの定義 ===============================================================
/**
 * @brief エリア
 */
class CountArea
{
	// クラス定数の宣言 -------------------------------------------------
public:
	static constexpr float AREA_HALF_HEIGHT = 1.0f;	// エリアの高さ(半分)

	// 条件
	enum class TriggerMode
	{
		ReachCount,	// 目標数以上入れる
		AllOut		// 全部外に出す
	};

	// スプライト数字の1文字分のサイズ
	static constexpr DirectX::SimpleMath::Vector2 NUMBER_SIZE = { 48.0f,72.0f };


	// データメンバの宣言 -----------------------------------------------
private:
	// 座標
	DirectX::SimpleMath::Vector3 m_position;

	// モード
	TriggerMode m_mode;

	// 目標数
	int m_targetNum;

	// 条件を達成しているか
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

	// エフェクト
	std::unique_ptr<AreaEffect> m_effect;

	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;

	std::unique_ptr<NumberRenderer3D> m_numberBorad;	// 内部の敵数描画


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CountArea(UserResources* pUR);

	// デストラクタ
	~CountArea();


// 操作
public:
	// 初期化処理
	void Initialize(
		CollisionManager* pCM,
		const DirectX::SimpleMath::Vector3& position, float x, float z,
		const std::function<void()>& operation, TriggerMode mode, int targetNum = 0);

	// 更新処理
	void Update(float elapsedTime, const DirectX::SimpleMath::Vector3& cameraPos, const DirectX::SimpleMath::Vector3& cameraUp);

	// 描画処理
	void Draw(const RenderContext& context, DebugFont* debugFont);

	// 終了処理
	void Finalize();


// 取得/設定
public:
	// 条件を達成しているかを取得
	bool IsTrigger() const { return m_isTrigger; }

// 内部実装
private:
	// 当たり判定の初期設定
	void SetupCollider(CollisionManager* pCM, float x, float z);

	// エリアに入った敵をリストに追加・条件判定
	void EnterEnemy(CollisionManager* pCM, uint32_t handle);

	// エリアを出た敵をリストから除外・条件判定
	void ExitEnemy(CollisionManager* pCM, uint32_t handle);

	// トリガーを起動
	void TriggerOn();
};
