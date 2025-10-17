/**
 * @file   CollisionManager.h
 *
 * @brief  コリジョンマネージャーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<functional>
#include"Source/Game/Common/Collision.h"


// クラスの定義 ===============================================================
/**
 * @brief CollisionManager
 */
class CollisionManager
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 判定の分類
	enum class Layer
	{
		NONE = -1,
		PlayerBody,		// プレイヤーの本体
		PlayerAttack,	// プレイヤーの攻撃
		EnemyBody,		// 敵の本体
		EnemyAttack,	// 敵の攻撃
		Stage,			// ステージオブジェクト
		Trigger,		// トリガー
		COUNT			// 分類の数
	};

	// 衝突判定フィルター
	struct LayerMatrix
	{
		bool matrix[(int)Layer::COUNT][(int)Layer::COUNT] = {};

		bool Test(Layer a, Layer b)const
		{
			return matrix[(int)a][(int)b];
		}
	};

	// コールバック
	struct Callbacks
	{
		// イベント
		std::function<void(uint32_t, uint32_t)> onEnter;
		std::function<void(uint32_t, uint32_t)> onExit;
		std::function<void(uint32_t, uint32_t)> onStay;
		// 押し出し
		std::function<void(
			uint32_t, const DirectX::SimpleMath::Vector3& normal, float depth)> onResolved;
	};

	// 当たり判定の形状
	enum class Type :uint8_t
	{
		NONE = -1,
		Sphere,
		OBB	
	};

	// 当たり判定の情報
	struct Desc
	{
		Type type = Type::NONE;		// 当たり判定の種類
		Layer layer = Layer::NONE;	// 衝突レイヤー
		bool isTrigger = false;		// イベントだけ→true
		bool isMultiHit = false;	// 連続ヒット有→true
		uint32_t userId = 0;		// 個別ID(敵などの識別用)
		SphereCollider* sphere = nullptr;	// 球の当たり判定のポインタ
		OBBCollider* obb = nullptr;			// OBBの当たり判定のポインタ
		DirectX::SimpleMath::Vector3* position = nullptr;	// 座標のポインタ
		DirectX::SimpleMath::Vector3* velocity = nullptr;	// 速度のポインタ
		Callbacks callback;			// コールバック
		float mass = 0.0f;			// 質量(静的オブジェクトなら0にしておく)
		float invMass = 0.0f;		// 質量の逆数
		float restitution = 0.8f;	// 反発係数
		float* userData = nullptr;	// 攻撃力などの個別データ
	};

	// 当たり判定のノード
	struct Node
	{
		uint32_t handle = 0;	// 登録時のハンドル
		Desc desc;				// 設定情報
		std::unordered_set<uint32_t> overlapsPrev;	// 前フレームに重なっていた相手のハンドル一覧
		std::unordered_set<uint32_t> overlapsNow;	// 現在重なっている相手のハンドル一覧
		bool alive = true;		// 生存フラグ
		bool enabled = true;	// 有効フラグ
	};

	// データメンバの宣言 -----------------------------------------------
private:
	// 次のハンドル
	uint32_t m_next;

	// 当たり判定の配列
	std::unordered_map<uint32_t, Node> m_nodes;

	// ハンドルの配列
	std::vector<uint32_t> m_order;

	// 衝突判定フィルター
	LayerMatrix m_matrix;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CollisionManager();

	// デストラクタ
	~CollisionManager() = default;


// 操作
public:
	// 更新処理
	void Update(float elapsedTime);

	// 追加
	uint32_t Add(const Desc& desc);

	// 除外
	void Remove(uint32_t handle);

	// 判定有効化設定
	void SetEnabled(uint32_t handle, bool enabled);

	// 判定有効フラグを取得
	bool IsEnabled(uint32_t handle);
		
	// 連続ヒット有効化設定
	void SetMultiHit(uint32_t handle, bool maltiHit);


// 取得/設定
public:
	// 当たり判定情報の取得
	const Desc* GetDesc(uint32_t handle) const;

	// 衝突判定フィルターの取得
	LayerMatrix& GetLayerMatrix() { return m_matrix; }


// 内部実装
private:
	// 速度をスライド
	void SlideVelocity(DirectX::SimpleMath::Vector3* velocity,
					   const DirectX::SimpleMath::Vector3& normal);

	// 衝突判定の解決
	// 球vsOBB
	void ResolveSphereVsOBB(Node& a, Node& b);	
	// 球vs球
	void ResolveSphereVsSphere(Node& a, Node& b);
	// OBBvsOBB
	void ResolveOBBVsOBB(Node& a, Node& b);
};
