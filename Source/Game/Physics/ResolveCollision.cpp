/**
 * @file   ResolveCollision.cpp
 *
 * @brief  衝突判定のめり込み解決に関するソースファイル
 */


// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ResolveCollision.h"
#include "Source/Game/Physics/PhysicsObject.h"

/**
 * @brief 球とOBBの衝突を解決する
 *
 * @param a 球コライダを保持するノード
 * @param b OBBコライダを保持するノード
 *
 * @return なし
 */
void ResolveSphereVsOBB(CollisionManager::Node& a, CollisionManager::Node& b)
{
	// 衝突判定を取得
	const SphereCollider* sphere = a.desc.sphere;
	const OBBCollider* obb = b.desc.obb;
	if (!obb || !sphere) return;

	// 衝突しているか
	if (!IsHit(*obb, *sphere)) return;

	MTV mtv = CalculateMTV(*obb, *sphere);

	if ((a.desc.isTrigger || b.desc.isTrigger) ||
		(mtv.distance <= 0.0f || mtv.direction.LengthSquared() <= FLT_EPSILON))
	{
		a.overlapsNow.insert(b.handle);
		b.overlapsNow.insert(a.handle);

		return;
	}

	// 方向を正規化
	DirectX::SimpleMath::Vector3 normal = mtv.direction;
	normal.Normalize();
	if (b.desc.layer == CollisionManager::Layer::Stage && normal.y < 0.0f)
	{
		normal = -normal;
	}

	float invA = a.desc.invMass;
	float invB = b.desc.invMass;
	float invSum = invA + invB;

	float depth = mtv.distance - PENETRATION_SLOP;
	if (depth < 0.0f) depth = 0.0f;

	if (invSum > 0.0f && depth > 0.0f)
	{
		DirectX::SimpleMath::Vector3 correction = normal * (depth * POSITION_CORRECTION_RATE / invSum);

		// 座標を調整
		if (a.desc.position && invA > 0.0f)
		{
			*a.desc.position += correction * invA;
			a.desc.sphere->SetCenter(*a.desc.position);
		}
		if (b.desc.position && invB > 0.0f)
		{
			*b.desc.position -= correction * invB;
			b.desc.obb->SetCenter(*b.desc.position);
		}
	}

	// 相対速度
	DirectX::SimpleMath::Vector3 velA = a.desc.velocity ? *a.desc.velocity : DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 velB = b.desc.velocity ? *b.desc.velocity : DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 rv = velA - velB;
	float vn = rv.Dot(normal);

	bool isGround = PhysicsObject::IsGroundNormal(normal);

	if (a.desc.layer == CollisionManager::Layer::PlayerBody && b.desc.layer == CollisionManager::Layer::Stage)
	{
		if (!isGround)
		{
			normal.y = 0.0f;
		}

		SlideVelocity(a.desc.velocity, normal);
	}
	else if (isGround)
	{
		// 地面の場合
		if (invSum > 0.0f && vn < 0.0f)
		{
			// インパルス
			float e = std::min(a.desc.restitution, b.desc.restitution);
			float j = -(1.0f + e) * vn / invSum;
			DirectX::SimpleMath::Vector3 impulse = j * normal;

			// 速度を調整
			if (a.desc.velocity && invA > 0.0f)
			{
				*a.desc.velocity += impulse * invA;
			}
			if (b.desc.velocity && invB > 0.0f)
			{
				*b.desc.velocity -= impulse * invB;
			}
		}
	}

	// 重なっている相手に加える
	a.overlapsNow.insert(b.handle);
	b.overlapsNow.insert(a.handle);

	// 解決時の処理を行う
	if (a.desc.callback.onResolved)
	{
		a.desc.callback.onResolved(b.handle, normal, mtv.distance);
	}
	if (b.desc.callback.onResolved)
	{
		b.desc.callback.onResolved(b.handle, normal, mtv.distance);
	}
}


/**
 * @brief 球と球の衝突を解決する
 *
 * @param a 球コライダを保持するノード
 * @param b 球コライダを保持するノード
 *
 * @return なし
 */
void ResolveSphereVsSphere(CollisionManager::Node& a, CollisionManager::Node& b)
{
	// 衝突判定を取得
	const SphereCollider* sphereA = a.desc.sphere;
	const SphereCollider* sphereB = b.desc.sphere;
	if (!sphereA || !sphereB) return;

	// 衝突しているか
	if (!IsHit(*sphereA, *sphereB)) return;

	MTV mtv = CalculateMTV(*sphereA, *sphereB);

	if ((a.desc.isTrigger || b.desc.isTrigger) ||
		(mtv.distance <= 0.0f || mtv.direction.LengthSquared() <= FLT_EPSILON))
	{
		a.overlapsNow.insert(b.handle);
		b.overlapsNow.insert(a.handle);

		return;
	}

	// 方向を正規化
	DirectX::SimpleMath::Vector3 normal = mtv.direction;
	normal.Normalize();

	float invA = a.desc.invMass;
	float invB = b.desc.invMass;
	float invSum = invA + invB;

	if (invSum > 0.0f)
	{

		DirectX::SimpleMath::Vector3 correction = normal * (mtv.distance * 0.8f / invSum);

		// 座標を調整
		if (a.desc.position && invA > 0.0f)
		{
			*a.desc.position -= correction * invA;
			a.desc.sphere->SetCenter(*a.desc.position);
		}
		if (b.desc.position && invB > 0.0f)
		{
			*b.desc.position += correction * invB;
			b.desc.sphere->SetCenter(*b.desc.position);
		}
	}

	// 相対速度
	DirectX::SimpleMath::Vector3 velA = a.desc.velocity ? *a.desc.velocity : DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 velB = b.desc.velocity ? *b.desc.velocity : DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 rv = velB - velA;
	float vn = rv.Dot(normal);

	if (invSum > 0.0f && vn < 0.0f)
	{
		float e = std::min(a.desc.restitution, b.desc.restitution);
		float j = -(1.0f + e) * vn / invSum;
		DirectX::SimpleMath::Vector3 impulse = j * normal;

		// 速度を調整
		if (a.desc.velocity && invA > 0.0f)
		{
			*a.desc.velocity -= impulse * invA;
		}
		if (b.desc.velocity && invB > 0.0f)
		{
			*b.desc.velocity += impulse * invB;
		}
	}

	// 重なっている相手に加える
	a.overlapsNow.insert(b.handle);
	b.overlapsNow.insert(a.handle);

	// 解決時の処理を行う
	if (a.desc.callback.onResolved)
	{
		a.desc.callback.onResolved(b.handle, normal, mtv.distance);
	}
	if (b.desc.callback.onResolved)
	{
		b.desc.callback.onResolved(b.handle, normal, mtv.distance);
	}
}


/**
 * @brief OBBとOBBの衝突を解決する
 *
 * @param a OBBコライダを保持するノード
 * @param b OBBコライダを保持するノード
 *
 * @return なし
 */
void ResolveOBBVsOBB(CollisionManager::Node& a, CollisionManager::Node& b)
{
	// 衝突判定を取得
	const OBBCollider* obbA = a.desc.obb;
	const OBBCollider* obbB = b.desc.obb;
	if (!obbA || !obbB) return;

	// 衝突しているか
	if (!IsHit(*obbA, *obbB)) return;

	MTV mtv = CalculateMTV(*obbB, *obbA);

	if ((a.desc.isTrigger || b.desc.isTrigger) ||
		(mtv.distance <= 0.0f || mtv.direction.LengthSquared() <= FLT_EPSILON))
	{
		a.overlapsNow.insert(b.handle);
		b.overlapsNow.insert(a.handle);

		return;
	}

	// 方向を正規化
	DirectX::SimpleMath::Vector3 normal = mtv.direction;
	normal.Normalize();

	// 座標を調整
	if (a.desc.position)
	{
		*a.desc.position += normal * mtv.distance;
		a.desc.obb->SetCenter(*a.desc.position);
	}

	// 速度を調整
	SlideVelocity(a.desc.velocity, normal);

	// 重なっている相手に加える
	a.overlapsNow.insert(b.handle);
	b.overlapsNow.insert(a.handle);

	// 解決時の処理を行う
	if (a.desc.callback.onResolved)
	{
		a.desc.callback.onResolved(b.handle, normal, mtv.distance);
	}
}

/**
 * @brief 法線方向の速度成分を除去し、スライド移動に変換
 *
 * @param velocity 移動速度ベクトル
 * @param normal   衝突面の法線ベクトル
 *
 * @return なし
 */
void SlideVelocity(
	DirectX::SimpleMath::Vector3* velocity,
	const DirectX::SimpleMath::Vector3& normal)
{
	if (!velocity) return;

	float vn = velocity->Dot(normal);
	if (vn < 0.0f) *velocity -= normal * vn;
}
