/**
 * @file   CollisionManager.h
 *
 * @brief  CollisionManagerに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
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
		PlayerBody,
		PlayerAttack,
		EnemyBody,
		EnemyAttack,
		Stage,
		Trigger,
		COUNT
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

	// 衝突判定の形状
	enum class Type :uint8_t
	{
		Sphere,
		OBB
	};

	// 情報
	struct Desc
	{
		Type type;
		Layer layer;
		bool isTrigger = false;		// イベントだけ→true
		bool isMultiHit = false;	// 連続ヒット有→true
		uint32_t userId = 0;
		SphereCollider* sphere = nullptr;
		OBBCollider* obb = nullptr;
		DirectX::SimpleMath::Vector3* position = nullptr;
		DirectX::SimpleMath::Vector3* velocity = nullptr;
		Callbacks callback;
		float mass = 0.0f;			// 質量(静的オブジェクトなら0にしておく)
		float invMass = 0.0f;		// 質量の逆数
		float restitution = 0.8f;	// 反発係数
		float* userData = nullptr;	// 攻撃力などの個別データ
	};

	struct Node
	{
		uint32_t handle;
		Desc desc;
		std::unordered_set<uint32_t> overlapsPrev;
		std::unordered_set<uint32_t> overlapsNow;
		bool alive = true;
		bool enabled = true;
	};

	// データメンバの宣言 -----------------------------------------------
private:
	uint32_t m_next;

	std::unordered_map<uint32_t, Node> m_nodes;

	std::vector<uint32_t> m_order;

	LayerMatrix m_matrix;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CollisionManager();

	// デストラクタ
	~CollisionManager();


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
	const Desc* GetDesc(uint32_t handle) const;
	LayerMatrix& Matrix() { return m_matrix; }


// 内部実装
private:
	void SlideVelocity(DirectX::SimpleMath::Vector3* velocity,
					   const DirectX::SimpleMath::Vector3& normal);

	// 衝突判定
	// 球vsOBB　球のみ押し出し
	void ResolveSphereVsOBB(Node& a, Node& b);	
	// 球vs球　両方押し出し
	void ResolveSphereVsSphere(Node& a, Node& b);
	// OBBvsOBB　A側のみ押し出し
	void ResolveOBBVsOBB(Node& a, Node& b);
};
