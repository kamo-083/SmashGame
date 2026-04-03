/**
 * @file   CollisionManager.cpp
 *
 * @brief  当たり判定マネージャーに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CollisionManager.h"
#include "Source/Game/Physics/ResolveCollision.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
CollisionManager::CollisionManager()
	: m_next{ 1 }
{
}

/**
 * @brief 更新処理
 *
 * @param なし
 *
 * @return なし
 */
void CollisionManager::Update()
{
	// 重なり具合の集合をクリア
	for (uint32_t handle : m_order)
	{
		auto it = m_nodes.find(handle);
		if (it == m_nodes.end() || !it->second.alive) continue;
		it->second.overlapsNow.clear();
	}

	// 総当たりで判定
	const size_t N = m_order.size();
	for (size_t i = 0; i < N; i++)
	{
		// 判定するオブジェクトAを取得
		auto itA = m_nodes.find(m_order[i]);
		if (itA == m_nodes.end() || !itA->second.alive) continue;
		Node& A = itA->second;

		// Aが有効でないならスキップ
		if (!A.enabled) continue;

		for (size_t j = i + 1; j < N; j++)
		{
			// 判定するオブジェクトBを取得
			auto itB = m_nodes.find(m_order[j]);
			if (itB == m_nodes.end() || !itB->second.alive) continue;
			Node& B = itB->second;

			// Bが有効でないならスキップ
			if (!B.enabled) continue;

			// レイヤーフィルターのチェック
			if (!m_matrix.Test(A.desc.layer, B.desc.layer) &&
				!m_matrix.Test(B.desc.layer, A.desc.layer))
				continue;

			// どちらかのisTriggerがtrueの場合、重なりのみ記録
			if (A.desc.isTrigger || B.desc.isTrigger)
			{
				bool hit = false;

				// トリガーの当たり判定
				if (A.desc.type == Type::Sphere && B.desc.type == Type::Sphere)		// 球同士
				{
					// 登録されていたら判定する
					if (A.desc.sphere && B.desc.sphere) hit = IsHit(*A.desc.sphere, *B.desc.sphere);
				}
				else if (A.desc.type == Type::Sphere && B.desc.type == Type::OBB)	// 球vsOBB
				{
					// 登録されていたら判定する
					if (A.desc.sphere && B.desc.obb) hit = IsHit(*B.desc.obb, *A.desc.sphere);
				}
				else if (A.desc.type == Type::OBB && B.desc.type == Type::Sphere)	// OBBvs球
				{
					// 登録されていたら判定する
					if (A.desc.obb && B.desc.sphere) hit = IsHit(*A.desc.obb, *B.desc.sphere);
				}
				else
				{
					// 登録されていたら判定する
					if (A.desc.obb && B.desc.obb) hit = IsHit(*A.desc.obb, *B.desc.obb);	// OBBvsOBB
				}

				// 当たっていたら重なりを記録
				if (hit)
				{
					A.overlapsNow.insert(B.handle);
					B.overlapsNow.insert(A.handle);
				}
				continue;
			}

			// オブジェクトの衝突判定
			if (A.desc.type == Type::Sphere)
			{
				if (B.desc.type == Type::Sphere) ResolveSphereVsSphere(A, B);	// 球vs球
				else							 ResolveSphereVsOBB(A, B);		// 球vsOBB
			}
			else
			{
				if (B.desc.type == Type::Sphere) ResolveSphereVsOBB(B, A);		// OBBvs球
				else							 ResolveOBBVsOBB(A, B);			// OBBvsOBB
			}
		}
	}

	// イベント
	for (uint32_t handle : m_order)
	{
		auto it = m_nodes.find(handle);
		if (it == m_nodes.end() || !it->second.alive) continue;
		Node& node = it->second;

		// Enter
		for (auto other : node.overlapsNow)
		{
			if (!node.overlapsPrev.count(other))
			{
				if (node.desc.callback.onEnter) node.desc.callback.onEnter(node.handle, other);
			}
		}

		// Exit
		for (auto other : node.overlapsPrev)
		{
			if (!node.overlapsNow.count(other))
			{
				if (node.desc.callback.onExit) node.desc.callback.onExit(node.handle, other);
			}
		}

		// Stay
		for (auto other : node.overlapsNow)
		{
			if (node.overlapsPrev.count(other))
			{
				if (node.desc.callback.onStay) node.desc.callback.onStay(node.handle, other);
			}
		}

		// NowからPrevに移動
		node.overlapsPrev = std::move(node.overlapsNow);
	}

	// aliveでないものを削除
	m_order.erase(std::remove_if(
		m_order.begin(),m_order.end(),
		[&](uint32_t handle) {
			auto it = m_nodes.find(handle);
			return it == m_nodes.end() || !it->second.alive;
		}),
		m_order.end());
	for (auto it = m_nodes.begin(); it != m_nodes.end();)
	{
		if (!it->second.alive) it = m_nodes.erase(it);
		else ++it;
	}
}

/**
 * @brief 管理する当たり判定の追加
 *
 * @param desc 追加する当たり判定の情報
 *
 * @return 当たり判定の管理ハンドル
 */
uint32_t CollisionManager::Add(const Desc& desc)
{
	const uint32_t h = m_next++;
	Node n;
	n.handle = h;
	n.desc = desc;
	n.alive = true;
	n.enabled = true;

	// 未入力の場合、質量の逆数を計算
	if (n.desc.invMass <= 0.0f)
	{	
		if (n.desc.mass > 0.0f) n.desc.invMass = 1.0f / desc.mass;
		else					n.desc.invMass = 0.0f;
	}

	m_nodes.emplace(h, std::move(n));
	m_order.push_back(h);

	return h;
}

/**
 * @brief 管理している当たり判定の除外
 *
 * @param handle 除外する当たり判定のハンドル
 *
 * @return なし
 */
void CollisionManager::Remove(uint32_t handle)
{
	auto it = m_nodes.find(handle);
	if (it != m_nodes.end())
	{
		it->second.alive = false;
	}
}

/**
 * @brief 指定した当たり判定の有効/無効化を設定
 *
 * @param handle  設定する当たり判定のハンドル
 * @param enabled 有効/無効
 *
 * @return なし
 */
void CollisionManager::SetEnabled(uint32_t handle, bool enabled)
{
	auto it = m_nodes.find(handle);
	if (it != m_nodes.end())
	{
		it->second.enabled = enabled;
	}
}

/**
 * @brief 指定した当たり判定の有効/無効を取得
 *
 * @param handle  取得する当たり判定のハンドル
 *
 * @retval true  有効
 * @retval false 無効
 */
bool CollisionManager::IsEnabled(uint32_t handle)
{
	auto it = m_nodes.find(handle);
	if (it != m_nodes.end())
	{
		return it->second.enabled;
	}
	return false;
}

/**
 * @brief 指定した当たり判定の連続ヒットの有効/無効を設定
 *
 * @param handle	取得する当たり判定のハンドル
 * @param multiHit  有効/無効
 *
 * @return なし
 */
void CollisionManager::SetMultiHit(uint32_t handle, bool multiHit)
{
	auto it = m_nodes.find(handle);
	if (it != m_nodes.end())
	{
		it->second.desc.isMultiHit = multiHit;
	}
}

/**
 * @brief 指定した当たり判定の情報のポインタを取得
 *
 * @param handle	取得する対象のハンドル
 *
 * @return 情報のポインタ
 */
const CollisionManager::Desc* CollisionManager::GetDesc(uint32_t handle) const
{
	auto it = m_nodes.find(handle);
	return (it == m_nodes.end()) ? nullptr : &it->second.desc;
}