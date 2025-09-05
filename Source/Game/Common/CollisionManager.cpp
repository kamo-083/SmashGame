/**
 * @file   CollisionManager.cpp
 *
 * @brief  CollisionManagerに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CollisionManager.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
CollisionManager::CollisionManager()
	:m_next{ 1 }
{

}



/**
 * @brief デストラクタ
 */
CollisionManager::~CollisionManager()
{

}


/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CollisionManager::Update(float elapsedTime)
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

				// 衝突判定
				if (A.desc.type == Type::Sphere && B.desc.type == Type::Sphere)
				{
					if (A.desc.sphere && B.desc.sphere) hit = IsHit(*A.desc.sphere, *B.desc.sphere);
				}
				else if (A.desc.type == Type::Sphere && B.desc.type == Type::OBB)
				{
					if (A.desc.sphere && B.desc.obb) hit = IsHit(*B.desc.obb, *A.desc.sphere);
				}
				else if (A.desc.type == Type::OBB && B.desc.type == Type::Sphere)
				{
					if (A.desc.obb && B.desc.sphere) hit = IsHit(*A.desc.obb, *B.desc.sphere);
				}
				else
				{
					if (A.desc.obb && B.desc.obb) hit = IsHit(*A.desc.obb, *B.desc.obb);
				}

				// 当たっていたら重なりを記録
				if (hit)
				{
					A.overlapsNow.insert(B.handle);
					B.overlapsNow.insert(A.handle);
				}
				continue;
			}

			// 衝突判定
			if (A.desc.type == Type::Sphere)
			{
				if (B.desc.type == Type::Sphere) ResolveSphereVsSphere(A, B);
				else							 ResolveSphereVsOBB(A, B);
			}
			else
			{
				if (B.desc.type == Type::Sphere) ResolveSphereVsOBB(B, A);
				else							 ResolveOBBVsOBB(A, B);
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


uint32_t CollisionManager::Add(const Desc& desc)
{
	const uint32_t h = m_next++;
	Node n;
	n.handle = h;
	n.desc = desc;
	n.alive = true;
	n.enabled = true;

	m_nodes.emplace(h, std::move(n));
	m_order.push_back(h);

	return h;
}


void CollisionManager::Remove(uint32_t handle)
{
	auto it = m_nodes.find(handle);
	if (it != m_nodes.end())
	{
		it->second.alive = false;
	}
}

void CollisionManager::SetEnabled(uint32_t handle, bool enabled)
{
	auto it = m_nodes.find(handle);
	if (it != m_nodes.end())
	{
		it->second.enabled = enabled;
	}
}

bool CollisionManager::IsEnabled(uint32_t handle)
{
	auto it = m_nodes.find(handle);
	if (it != m_nodes.end())
	{
		return it->second.enabled;
	}
}

void CollisionManager::SetMultiHit(uint32_t handle, bool multiHit)
{
	auto it = m_nodes.find(handle);
	if (it != m_nodes.end())
	{
		it->second.desc.isMultiHit = multiHit;
	}
}


const CollisionManager::Desc* CollisionManager::GetDesc(uint32_t handle) const
{
	auto it = m_nodes.find(handle);
	return (it == m_nodes.end()) ? nullptr : &it->second.desc;
}


void CollisionManager::SlideVelocity(DirectX::SimpleMath::Vector3* velocity, 
									 const DirectX::SimpleMath::Vector3& normal)
{
	if (!velocity) return;

	float vn = velocity->Dot(normal);
	if (vn < 0.0f) *velocity -= normal * vn;
}

void CollisionManager::ResolveSphereVsOBB(Node& a, Node& b)
{
	// 当たり判定を取得
	const SphereCollider* sphere = a.desc.sphere;
	const OBBCollider* obb = b.desc.obb;
	if (!obb || !sphere) return;

	// 衝突しているか
	if (!IsHit(*obb, *sphere)) return;

	MTV mtv = CalculateMTV(*obb, *sphere);

	if ((a.desc.isTrigger || b.desc.isTrigger) ||
		(mtv.distance <= 0.0f || mtv.direction.LengthSquared() <= 0.00001f))
	{
		a.overlapsNow.insert(b.handle);
		b.overlapsNow.insert(a.handle);

		return;
	}

	// 方向を正規化
	SimpleMath::Vector3 normal = mtv.direction;
	normal.Normalize();
	
	// 座標を調整
	if (a.desc.position)
	{
		*a.desc.position += normal * mtv.distance;
		a.desc.sphere->SetCenter(*a.desc.position);
	}

	// 速度を調整
	SlideVelocity(a.desc.velocity, normal);

	a.overlapsNow.insert(b.handle);
	b.overlapsNow.insert(a.handle);

	if (a.desc.callback.onResolved)
	{
		a.desc.callback.onResolved(b.handle, normal, mtv.distance);
	}
}


void CollisionManager::ResolveSphereVsSphere(Node& a, Node& b)
{
	// 当たり判定を取得
	const SphereCollider* sphereA = a.desc.sphere;
	const SphereCollider* sphereB = b.desc.sphere;
	if (!sphereA || !sphereB) return;

	// 衝突しているか
	if (!IsHit(*sphereA, *sphereB)) return;

	MTV mtv = CalculateMTV(*sphereA, *sphereB);

	if ((a.desc.isTrigger || b.desc.isTrigger) ||
		(mtv.distance <= 0.0f || mtv.direction.LengthSquared() <= 0.00001f))
	{
		a.overlapsNow.insert(b.handle);
		b.overlapsNow.insert(a.handle);

		return;
	}

	// 方向を正規化
	SimpleMath::Vector3 normal = mtv.direction;
	normal.Normalize();

	float depth = mtv.distance * 0.5f;

	// 座標を調整
	if (a.desc.position)
	{
		*a.desc.position -= normal * depth;
		a.desc.sphere->SetCenter(*a.desc.position);
	}
	if (b.desc.position)
	{
		*b.desc.position += normal * depth;
		b.desc.sphere->SetCenter(*b.desc.position);
	}

	// 速度を調整
	SlideVelocity(a.desc.velocity, normal);
	SlideVelocity(b.desc.velocity, -normal);

	a.overlapsNow.insert(b.handle);
	b.overlapsNow.insert(a.handle);

	if (a.desc.callback.onResolved)
	{
		a.desc.callback.onResolved(b.handle, normal, depth);
	}
	if (b.desc.callback.onResolved)
	{
		b.desc.callback.onResolved(a.handle, -normal, depth);
	}
}


void CollisionManager::ResolveOBBVsOBB(Node& a, Node& b)
{
	// 当たり判定を取得
	const OBBCollider* obbA = a.desc.obb;
	const OBBCollider* obbB = b.desc.obb;
	if (!obbA || !obbB) return;

	// 衝突しているか
	if (!IsHit(*obbA, *obbB)) return;

	MTV mtv = CalculateMTV(*obbB, *obbA);

	if ((a.desc.isTrigger || b.desc.isTrigger) ||
		(mtv.distance <= 0.0f || mtv.direction.LengthSquared() <= 0.00001f))
	{
		a.overlapsNow.insert(b.handle);
		b.overlapsNow.insert(a.handle);

		return;
	}

	// 方向を正規化
	SimpleMath::Vector3 normal = mtv.direction;
	normal.Normalize();

	// 座標を調整
	if (a.desc.position)
	{
		*a.desc.position += normal * mtv.distance;
		a.desc.obb->SetCenter(*a.desc.position);
	}

	// 速度を調整
	SlideVelocity(a.desc.velocity, normal);

	a.overlapsNow.insert(b.handle);
	b.overlapsNow.insert(a.handle);

	if (a.desc.callback.onResolved)
	{
		a.desc.callback.onResolved(b.handle, normal, mtv.distance);
	}
}
