/**
 * @file   Collision.cpp
 *
 * @brief  球・OBBの当たり判定に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Collision.h"


////球のコライダー /////////////////////////////////////////
/**
 * @brief コンストラクタ
 */
SphereCollider::SphereCollider()
	:
	m_center{ 0.0f,0.0f,0.0f },
	m_radius{ 0.0f }
{
}

/**
 * @brief コンストラクタ
 *
 * @param center 球の中心座標
 * @param radius 球の半径
 */
SphereCollider::SphereCollider(const DirectX::SimpleMath::Vector3& center, float radius)
	: 
	m_center{ center },
	m_radius{ radius }
{
}



////OBBのコライダー /////////////////////////////////////////
/**
 * @brief コンストラクタ
 */
OBBCollider::OBBCollider()
{
	m_obb.center = DirectX::SimpleMath::Vector3::Zero;
	m_obb.rotation = DirectX::SimpleMath::Quaternion::Identity;
	m_obb.halfLength = DirectX::SimpleMath::Vector3::Zero;

	m_mtv.direction = DirectX::SimpleMath::Vector3::Zero;
	m_mtv.distance = 0.0f;
}

/**
 * @brief コンストラクタ
 *
 * @param center	 OBBの中心座標
 * @param direction	 OBBの方向ベクトル
 * @param halfLength OBBの中心座標から面までの長さ
 */
OBBCollider::OBBCollider(
	const DirectX::SimpleMath::Vector3& center,
	const DirectX::SimpleMath::Quaternion& rotation,
	const DirectX::SimpleMath::Vector3& halfLength)
{
	m_obb.center = center;
	m_obb.halfLength = halfLength;
	m_obb.rotation = rotation;
	AxisFromQuaternion(m_obb.rotation, m_obb.axis);

	m_mtv.direction = DirectX::SimpleMath::Vector3::Zero;
	m_mtv.distance = 0.0f;
}


/**
 * @brief OBBの中心座標の取得
 *
 * @param 取得したい軸の番号 (x=0, y=1, z=2)
 *
 * @return 中心座標
 */
float OBBCollider::GetCenter(int n) const
{
	switch (n)
	{
	case 0:	 return m_obb.center.x;
	case 1:	 return m_obb.center.y;
	case 2:  return m_obb.center.z;
	}
	return 0.0f;
}

/**
 * @brief OBBの中心座標から面までの長さの取得
 *
 * @param n 取得したい軸の番号(x=0, y=1, z=2)
 *
 * @return 中心座標から面までの長さ
 */
float OBBCollider::GetHalfLength(int n) const
{
	switch (n)
	{
	case 0:	 return m_obb.halfLength.x;
	case 1:	 return m_obb.halfLength.y;
	case 2:   return m_obb.halfLength.z;
	}
	return 0.0f;
}

void OBBCollider::SetRotation(const DirectX::SimpleMath::Quaternion& rotation)
{
	m_obb.rotation = rotation;
	AxisFromQuaternion(m_obb.rotation, m_obb.axis);
}



////当たり判定の処理 /////////////////////////////////////////
/**
 * @brief 球と球の当たり判定
 *
 * @param sphereA 判定対象の球A
 * @param sphereB 判定対象の球B
 *
 * @retval true  衝突している
 * @retval false 衝突していない
 */
bool IsHit(const SphereCollider& sphereA, const SphereCollider& sphereB)
{
	// 2つの球の距離を求める
	float distance = Distance(sphereA, sphereB);

	//半径の和を求める
	float radiusSum = sphereA.GetRadius() + sphereB.GetRadius();

	// 2つの球の距離が半径の和よりも小さかったら当たっている
	if (distance < radiusSum) return true;

	return false;
}


/**
 * @brief OBBの当たり判定
 *
 * @param sphereA 判定対象のOBB A
 * @param sphereB 判定対象のOBB B
 *
 * @retval true  衝突している
 * @retval false 衝突していない
 */
bool IsHit(const OBBCollider& obbA, const OBBCollider& obbB)
{
	//OBBの軸とサイズを取得 (方向ベクトルは正規化されていること)
	DirectX::SimpleMath::Vector3 axisA[3], axisB[3];
	for (int i = 0; i < 3; i++)
	{
		axisA[i] = obbA.GetAxis(i);
		axisB[i] = obbB.GetAxis(i);
	}

	// OBB Aの各ローカル軸ベクトルを計算
	DirectX::SimpleMath::Vector3 extentA[3] =
	{
		axisA[0] * obbA.GetHalfLength().x,
		axisA[1] * obbA.GetHalfLength().y,
		axisA[2] * obbA.GetHalfLength().z
	};

	// OBB Bの各ローカル軸ベクトルを計算
	DirectX::SimpleMath::Vector3 extentB[3] =
	{
		axisB[0] * obbB.GetHalfLength().x,
		axisB[1] * obbB.GetHalfLength().y,
		axisB[2] * obbB.GetHalfLength().z
	};

	//中心座標の距離
	DirectX::SimpleMath::Vector3 centerInterval = obbB.GetCenter() - obbA.GetCenter();

	// 関数に渡す用のダミー
	float dummyOverlap = FLT_MAX;
	DirectX::SimpleMath::Vector3 dummyAxis;

	for (int i = 0; i < 3; i++)
	{
		//分離軸：Aのローカル軸
		if (!TryAxis(axisA[i], centerInterval, extentA, extentB, false, dummyOverlap, dummyAxis))return false;

		//分離軸：Bのローカル軸
		if (!TryAxis(axisB[i], centerInterval, extentA, extentB, false, dummyOverlap, dummyAxis))return false;
	}

	//分離軸：ローカル軸同士のクロス積で得られる軸
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			DirectX::SimpleMath::Vector3 cross = axisA[i].Cross(axisB[j]);
			if (!TryAxis(cross, centerInterval, extentA, extentB, false, dummyOverlap, dummyAxis))return false;
		}
	}

	return true;		//衝突している (分離平面が存在しない)
}

/**
 * @brief OBBと球の当たり判定
 *
 * @param obb	 判定対象のOBB
 * @param sphere 判定対象の球
 *
 * @retval true  衝突している
 * @retval false 衝突していない
 */
bool IsHit(const OBBCollider& obb, const SphereCollider& sphere)
{
	DirectX::SimpleMath::Vector3 vec{ 0.0f, 0.0f, 0.0f };
	DirectX::SimpleMath::Vector3 delta = sphere.GetCenter() - obb.GetCenter();

	//各軸についてはみ出た部分のベクトルを算出
	for (int i = 0; i < 3; i++)
	{
		float L = obb.GetHalfLength(i);

		if (L <= 0) continue;

		float s = delta.Dot(obb.GetAxis(i)) / L;	//投影距離/半径

		s = fabs(s);

		if (s > 1.0f)
		{
			vec += (1.0f - s) * L * obb.GetAxis(i);
		}
	}

	//はみ出し距離の合計ベクトルの長さ(距離)
	float length = vec.Length();

	//最短距離が球の半径より短かったら衝突している
	if (sphere.GetRadius() >= length)
	{
		return true;
	}

	return false;
}


/**
 * @brief 球と球の最短距離を計算
 *
 * @param sphereA 判定対象の球A
 * @param sphereB 判定対象の球B
 *
 * @return 球と球の最短距離
 */
float Distance(const SphereCollider& sphereA, const SphereCollider& sphereB)
{
	float x = sphereB.GetCenter().x - sphereA.GetCenter().x;
	float y = sphereB.GetCenter().y - sphereA.GetCenter().y;
	float z = sphereB.GetCenter().z - sphereA.GetCenter().z;

	return sqrtf(x * x + y * y + z * z);
}


/**
 * @brief 球と球の最小押し出しベクトル(MTV)を計算
 *
 * @param obbA 判定対象の球A
 * @param obbB 判定対象の球B
 *
 * @return 衝突を解消するための最小押し出しベクトル(MTV)
 */
MTV CalculateMTV(const SphereCollider& sphereA, const SphereCollider& sphereB)
{
	MTV mtv;

	DirectX::SimpleMath::Vector3 direction = sphereB.GetCenter() - sphereA.GetCenter();
	mtv.direction = direction;

	float centerDistance = mtv.direction.Length();
	float radiusSum = sphereA.GetRadius() + sphereB.GetRadius();
	mtv.distance = radiusSum - centerDistance;

	return mtv;
}


/**
 * @brief OBBとOBBの最小押し出しベクトル(MTV)を計算
 *
 * @param obbA 判定対象のOBB A
 * @param obbB 判定対象のOBB B
 *
 * @return 衝突を解消するための最小押し出しベクトル(MTV)
 */
MTV CalculateMTV(const OBBCollider& obbA, const OBBCollider& obbB)
{
	MTV mtv;
	mtv.distance = FLT_MAX;

	//OBBの軸とサイズを取得 (方向ベクトルは正規化されていること)
	DirectX::SimpleMath::Vector3 axisA[3], axisB[3];
	for (int i = 0; i < 3; i++)
	{
		axisA[i] = obbA.GetAxis(i);
		axisB[i] = obbB.GetAxis(i);
	}

	DirectX::SimpleMath::Vector3 extentA[3] =
	{
		axisA[0] * obbA.GetHalfLength().x,
		axisA[1] * obbA.GetHalfLength().y,
		axisA[2] * obbA.GetHalfLength().z
	};

	DirectX::SimpleMath::Vector3 extentB[3] =
	{
		axisB[0] * obbB.GetHalfLength().x,
		axisB[1] * obbB.GetHalfLength().y,
		axisB[2] * obbB.GetHalfLength().z
	};

	//中心座標の距離
	DirectX::SimpleMath::Vector3 centerInterval = obbB.GetCenter() - obbA.GetCenter();

	for (int i = 0; i < 3; i++)
	{
		//分離軸：Aのローカル軸
		TryAxis(axisA[i], centerInterval, extentA, extentB, true, mtv.distance, mtv.direction);

		//分離軸：Bのローカル軸
		TryAxis(axisB[i], centerInterval, extentA, extentB, true, mtv.distance, mtv.direction);
	}

	//分離軸：ローカル軸同士のクロス積で得られる軸
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			DirectX::SimpleMath::Vector3 cross = axisA[i].Cross(axisB[j]);
			TryAxis(cross, centerInterval, extentA, extentB, true, mtv.distance, mtv.direction);
		}
	}

	return mtv;
}


/**
 * @brief OBBと球の最小押し出しベクトル(MTV)を計算
 *
 * @param obb     判定対象のOBB
 * @param sphere  判定対象の球
 *
 * @return 衝突を解消するための最小押し出しベクトル(MTV)
 */
MTV CalculateMTV(const OBBCollider& obb, const SphereCollider& sphere)
{
	MTV mtv = { DirectX::SimpleMath::Vector3::Zero ,FLT_MAX };

	//球の中心からOBB上の最近傍点を求める
	DirectX::SimpleMath::Vector3 delta = sphere.GetCenter() - obb.GetCenter();
	DirectX::SimpleMath::Vector3 closest = obb.GetCenter();

	for (int i = 0; i < 3; i++)
	{
		float dist = delta.Dot(obb.GetAxis(i));
		float clamped = std::max(-obb.GetHalfLength(i), std::min(dist, obb.GetHalfLength(i)));
		closest += obb.GetAxis(i) * clamped;
	}

	//最近傍点と球の中心との距離を求める
	DirectX::SimpleMath::Vector3 displacement = sphere.GetCenter() - closest;
	float distance = displacement.Length();
	float penetration = sphere.GetRadius() - distance;

	// 外側
	if (penetration > 0.0f && distance > 0.0001f)
	{
		//押し出しベクトルを計算
		mtv.direction = displacement / distance;	//正規化
		mtv.distance = penetration;

		return mtv;
	}

	// 内側
	float proj[3] =
	{
		delta.Dot(obb.GetAxis(0)),
		delta.Dot(obb.GetAxis(1)),
		delta.Dot(obb.GetAxis(2))
	};

	float gap[3] =
	{
		obb.GetHalfLength(0) - std::fabs(proj[0]),
		obb.GetHalfLength(1) - std::fabs(proj[1]),
		obb.GetHalfLength(2) - std::fabs(proj[2])
	};

	// 最小移動量と向きを求める
	int minAxis = 0;
	float minMove = gap[0] + sphere.GetRadius();
	for (int i = 1; i < 3; ++i)
	{
		float move = gap[i] + sphere.GetRadius();
		if (move < minMove)
		{
			minMove = move;
			minAxis = i;
		}
	}

	DirectX::SimpleMath::Vector3 dir = (proj[minAxis] >= 0.0f ? obb.GetAxis(minAxis) : -obb.GetAxis(minAxis));
	mtv.direction = dir;
	mtv.distance = minMove;

	return mtv;
}


/**
 * @brief 平面と球の中心との距離の算出
 *
 * @param plane	 平面
 * @param center 球の中心座標
 *
 * @return 球の中心から平面までの最短距離
 */
float Distance(const DirectX::SimpleMath::Plane& plane,
			   const DirectX::SimpleMath::Vector3& center)
{
	//平面の方程式
	// Ax+By+Cz+D=0
	// 
	//距離の計算 面の法線ベクトル=(A,B,C),面のオフセット=D,球の中心座標=(x,y,z)
	// 
	//		｜Ax+By+Cz+D｜
	// d = ────────
	//		  √A²+B²+C²

	float x = plane.x * center.x;
	float y = plane.y * center.y;
	float z = plane.z * center.z;
	float numerator = fabs(x + y + z + plane.D());	//分子

	float a = plane.x * plane.x;
	float b = plane.y * plane.y;
	float c = plane.z * plane.z;
	float denominator = (sqrtf(a + b + c));			//分母

	return numerator / denominator;
}


/**
 * @brief 指定された軸で分離軸を判定
 *
 * @param axisRaw			判定に使用する軸ベクトル（正規化前）
 * @param centerInterval	2つのOBBの中心間ベクトル
 * @param extentA			OBB Aの各ローカル軸ベクトル（正規化済み×半径）
 * @param extentB			OBB Bの各ローカル軸ベクトル（正規化済み×半径）
 * @param useMTV			MTVを更新するか
 * @param minOverlap		MTVに渡す最小重なり量
 * @param bestAxis			MTVに渡す方向
 *
 * @retval true  分離している
 * @retval false 分離していない
 */
bool TryAxis(const DirectX::SimpleMath::Vector3& axisRaw,
			 const DirectX::SimpleMath::Vector3& centerInterval,
			 const DirectX::SimpleMath::Vector3 extentA[3],
			 const DirectX::SimpleMath::Vector3 extentB[3],
			 bool useMTV,
			 float& minOverlap,
			 DirectX::SimpleMath::Vector3& bestAxis)
{
	// 無効な値はスキップ
	if (axisRaw.LengthSquared() < 0.00001)	return true;

	DirectX::SimpleMath::Vector3 axis = axisRaw;
	axis.Normalize();

	float rA = fabs(axis.Dot(extentA[0])) + fabs(axis.Dot(extentA[1])) + fabs(axis.Dot(extentA[2]));
	float rB = fabs(axis.Dot(extentB[0])) + fabs(axis.Dot(extentB[1])) + fabs(axis.Dot(extentB[2]));
	float distance = fabs(centerInterval.Dot(axis));
	float overlap = rA + rB - distance;

	if (overlap < 0.0f) return false;

	if (useMTV && overlap < minOverlap)
	{
		minOverlap = overlap;
		bestAxis = (centerInterval.Dot(axis) < 0.0f) ? -axis : axis;
	}

	return true;
}


/**
 * @brief 指定された軸上での2つのOBBの投影半径を計算
 *
 * @param axisA   判定に使用する軸ベクトル（正規化前でも可）
 * @param extentA OBB Aの各軸方向のスケール（半径ベクトル）
 * @param extentB OBB Bの各ローカル軸ベクトル（正規化済み×半径）
 *
 * @return 2つのOBBを指定軸に投影した際の合計半径
 */
float CalculateProjectionRadius(
	const DirectX::SimpleMath::Vector3& axisA,
	const DirectX::SimpleMath::Vector3& extentA,
	const DirectX::SimpleMath::Vector3  extentB[3])
{
	float rA = extentA.Length();
	float rB = LenSegOnSeparateAxis(axisA, extentB[0], extentB[1], extentB[2]);

	return rA + rB;
}


/**
 * @brief 分離軸に対するOBBの投影半径を算出
 *
 * @param sep 分離軸（標準化済みの単位ベクトル）
 * @param e1  OBBの第1軸ベクトル（正規化済み×半径）
 * @param e2  OBBの第2軸ベクトル（正規化済み×半径）
 * @param e3  OBBの第3軸ベクトル（正規化済み×半径）※ 0でも可
 *
 * @return 分離軸上でのOBBの投影半径（線分長の半分）
 */
float LenSegOnSeparateAxis(
	const DirectX::SimpleMath::Vector3& sep,
	const DirectX::SimpleMath::Vector3& e1,
	const DirectX::SimpleMath::Vector3& e2,
	const DirectX::SimpleMath::Vector3& e3)
{
	//3つの内積の絶対値の和で投影線分長を計算
	//分離軸Sepは標準化されていること
	float r1 = fabs(sep.Dot(e1));
	float r2 = fabs(sep.Dot(e2));
	float r3 = e3 != DirectX::SimpleMath::Vector3::Zero ? (fabs(sep.Dot(e3))) : 0.0f;

	return r1 + r2 + r3;
}


/**
 * @brief 3点から平面を算出する
 *
 * @param p1 平面上の1点目
 * @param p2 平面上の2点目
 * @param p3 平面上の3点目
 * 
 * @return 3点を通る平面
 */
DirectX::SimpleMath::Plane CalculatePlane(
	const DirectX::SimpleMath::Vector3& p1,
	const DirectX::SimpleMath::Vector3& p2,
	const DirectX::SimpleMath::Vector3& p3)
{
	DirectX::SimpleMath::Vector3 v1 = p2 - p1;
	DirectX::SimpleMath::Vector3 v2 = p3 - p1;
	DirectX::SimpleMath::Vector3 normal = v1.Cross(v2);	//外積を求める
	normal.Normalize();							//正規化

	float D = -(normal.Dot(p1));

	return DirectX::SimpleMath::Plane(normal.x, normal.y, normal.z, D);
}


/**
 * @brief クォータニオンから各軸ベクトルを算出
 *
 * @param rotation 回転を表すクォータニオン
 * @param axis     回転後の各軸ベクトルを格納する配列(axis[0]=X軸, axis[1]=Y軸, axis[2]=Z軸)
 * 
 * @return なし
 */
void AxisFromQuaternion(const DirectX::SimpleMath::Quaternion& rotation, DirectX::SimpleMath::Vector3* axis)
{
	DirectX::SimpleMath::Matrix rotMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);

	axis[0] = DirectX::SimpleMath::Vector3(rotMatrix._11, rotMatrix._12, rotMatrix._13);	// X軸
	axis[1] = DirectX::SimpleMath::Vector3(rotMatrix._21, rotMatrix._22, rotMatrix._23);	// Y軸
	axis[2] = DirectX::SimpleMath::Vector3(rotMatrix._31, rotMatrix._32, rotMatrix._33);	// Z軸
}


/**
 * @brief 衝突面の法線から衝突面の種類を判定
 *
 * @param normal 衝突面の法線ベクトル（正規化済み想定）
 * 
 * @return 判定された衝突面の種類（OBBCollider::CollisionType）
 */
OBBCollider::CollisionType DetermineCollisionType(const DirectX::SimpleMath::Vector3& normal)
{
	if (normal.y > 0.9f)
	{
		return OBBCollider::CollisionType::Ground;
	}
	else if (std::abs(normal.y) > 0.5f)
	{
		return OBBCollider::CollisionType::Slope;
	}
	else if (std::abs(normal.x) > 0.9f || std::abs(normal.z) > 0.9f)
	{
		return OBBCollider::CollisionType::Wall;
	}
	return OBBCollider::CollisionType::Others;
}
