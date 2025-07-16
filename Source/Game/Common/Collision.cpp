#include "pch.h"
#include "Collision.h"

using namespace DirectX;

////球のコライダー /////////////////////////////////////////
/**
 * @brief コンストラクタ
 */
SphereCollider::SphereCollider()
	: m_center{ 0.0f,0.0f,0.0f }
	, m_radius{ 0.0f }
{
}

/**
 * @brief コンストラクタ
 *
 * @param[in] center 球の中心座標
 * @param[in] radius 球の半径
 */
SphereCollider::SphereCollider(SimpleMath::Vector3 center, float radius)
	: m_center{ center }
	, m_radius{ radius }
{
}

/**
 * @brief デストラクタ
 */
SphereCollider::~SphereCollider()
{
}

/**
 * @brief 球の中心座標の取得
 *
 * @param[in] なし 
 *
 * @return 球の中心座標
 */
SimpleMath::Vector3 SphereCollider::GetCenter() const
{
	return m_center;
}

/**
 * @brief 球の半径の取得
 *
 * @param[in] なし
 *
 * @return 半径
 */
float SphereCollider::GetRadius() const
{
	return m_radius;
}

/**
 * @brief 球の中心座標の設定
 *
 * @param[in] center 球の中心座標
 *
 * @return なし
 */
void SphereCollider::SetCenter(SimpleMath::Vector3 center)
{
	m_center = center;
}

/**
 * @brief 球の半径の設定
 *
 * @param[in] radius 球の半径
 *
 * @return なし
 */
void SphereCollider::SetRadius(float radius)
{
	m_radius = radius;
}


////直方体のコライダー /////////////////////////////////////////
/**
 * @brief コンストラクタ
 */
OBBCollider::OBBCollider()
{
	m_obb.center = { 0.0f, 0.0f, 0.0f };
	m_obb.axis[0] = { 0.0f, 0.0f, 0.0f };
	m_obb.axis[1] = { 0.0f, 0.0f, 0.0f };
	m_obb.axis[2] = { 0.0f, 0.0f, 0.0f };
	m_obb.halfLength = { 0.0f, 0.0f, 0.0f };

	m_mtv.direction = { 0.0f, 0.0f, 0.0f };
	m_mtv.distance = 0.0f;
}

/**
 * @brief コンストラクタ
 *
 * @param[in] center	 直方体の中心座標
 * @param[in] direction	 直方体の方向ベクトル
 * @param[in] halfLength 直方体の中心座標から面までの長さ
 */
OBBCollider::OBBCollider(SimpleMath::Vector3 center, SimpleMath::Vector3 axis[3], SimpleMath::Vector3 halfLength)
{
	m_obb.center = center;
	m_obb.axis[0] = axis[0];
	m_obb.axis[1] = axis[1];
	m_obb.axis[2] = axis[2];
	m_obb.halfLength = halfLength;

	m_mtv.direction = { 0.0f, 0.0f, 0.0f };
	m_mtv.distance = 0.0f;
}

/**
 * @brief デストラクタ
 */
OBBCollider::~OBBCollider()
{
}

/**
 * @brief 直方体の中心座標の取得
 *
 * @param[in] なし
 *
 * @return 中心座標
 */
SimpleMath::Vector3 OBBCollider::GetCenter() const
{
	return m_obb.center;
}

/**
 * @brief 直方体の方向ベクトルの取得
 *
 * @param[in] なし
 *
 * @return 方向ベクトル
 */
SimpleMath::Vector3 OBBCollider::GetAxes(int n) const
{
	return m_obb.axis[n];
}

/**
 * @brief 直方体の中心座標から面までの長さの取得
 *
 * @param[in] なし
 *
 * @return 中心座標から面までの長さ
 */
SimpleMath::Vector3 OBBCollider::GetHalfLength() const
{
	return m_obb.halfLength;
}

/**
 * @brief 直方体の中心座標から面までの長さの取得
 *
 * @param[in] 取得したい軸の番号 (x=0, y=1, z=2)
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
	default: return 0.0f;
	}
}

/**
 * @brief 直方体の中心座標の設定
 *
 * @param[in] center 中心座標
 *
 * @return なし
 */
void OBBCollider::SetCenter(SimpleMath::Vector3 center)
{
	m_obb.center = center;
}

/**
 * @brief 直方体の方向ベクトルの設定
 *
 * @param[in] axis 方向ベクトル
 *
 * @return なし
 */
void OBBCollider::SetAxes(SimpleMath::Vector3 axis[3])
{
	m_obb.axis[0] = axis[0];
	m_obb.axis[1] = axis[1];
	m_obb.axis[2] = axis[2];
}

void OBBCollider::SetAxes(SimpleMath::Vector3 axis)
{
	float yaw = XMConvertToRadians(axis.y);
	float pitch = XMConvertToRadians(axis.x);
	float roll = XMConvertToRadians(axis.z);
	//回転行列を作成
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
	//回転後の軸ベクトルを代入
	m_obb.axis[0] = rot.Right();    // X軸
	m_obb.axis[1] = rot.Up();       // Y軸
	m_obb.axis[2] = rot.Forward();  // Z軸
}

/**
 * @brief 直方体の中心座標から面までの長さの設定
 *
 * @param[in] center 中心座標から面までの長さ
 *
 * @return なし
 */
void OBBCollider::SetHalfLength(SimpleMath::Vector3 halfLength)
{
	m_obb.halfLength = halfLength;
}


////当たり判定の処理 /////////////////////////////////////////
/**
 * @brief 球と球の当たり判定
 *
 * @param[in] sphereA 判定対象の球A
 * @param[in] sphereB 判定対象の球B
 *
 * @retval true  衝突している場合
 * @retval false 衝突していない場合
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
 * @param[in] sphereA 判定対象の直方体A
 * @param[in] sphereB 判定対象の直方体B
 *
 * @retval true  衝突している場合
 * @retval false 衝突していない場合
 */
bool IsHit(const OBBCollider& obbA, const OBBCollider& obbB)
{
	//OBBの軸とサイズを取得 (方向ベクトルは正規化されていること)
	SimpleMath::Vector3 axisA[3] =
	{
		obbA.GetAxes(0),
		obbA.GetAxes(1),
		obbA.GetAxes(2)
	};
	SimpleMath::Vector3 extentA[3] =
	{
		axisA[0] * obbA.GetHalfLength().x,
		axisA[1] * obbA.GetHalfLength().y,
		axisA[2] * obbA.GetHalfLength().z
	};
	SimpleMath::Vector3 axisB[3] =
	{
		obbB.GetAxes(0),
		obbB.GetAxes(1),
		obbB.GetAxes(2)
	};
	SimpleMath::Vector3 extentB[3] =
	{
		axisB[0]* obbB.GetHalfLength().x,
		axisB[1]* obbB.GetHalfLength().y,
		axisB[2]* obbB.GetHalfLength().z
	};

	//中心座標の距離
	SimpleMath::Vector3 centerInterval =
	{
		obbA.GetCenter().x - obbB.GetCenter().x,
		obbA.GetCenter().y - obbB.GetCenter().y,
		obbA.GetCenter().z - obbB.GetCenter().z
	};

	//15本の分離軸をチェック
	//分離軸：Aのローカル軸
	for (int i = 0; i < 3; i++)
	{
		float r = CalculateProjectionRadius(axisA[i], extentA[i], extentB);
		float L = fabs(centerInterval.Dot(axisA[i]));
		if (L > r) return false;	//衝突していない
	}

	//分離軸：Bのローカル軸
	for (int i = 0; i < 3; i++)
	{
		float r = CalculateProjectionRadius(axisB[i], extentB[i], extentA);
		float L = fabs(centerInterval.Dot(axisB[i]));
		if (L > r) return false;	//衝突していない
	}

	//分離軸：ローカル軸同士のクロス積で得られる軸
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			SimpleMath::Vector3 cross = axisA[i].Cross(axisB[j]);

			float rA, rB, L;
			
			if (i == 0)		 rA = LenSegOnSeparateAxis(&cross, &extentA[1], &extentA[2]);
			else if (i == 1) rA = LenSegOnSeparateAxis(&cross, &extentA[0], &extentA[2]);
			else			 rA = LenSegOnSeparateAxis(&cross, &extentA[0], &extentA[1]);

			if (j == 0)		 rB = LenSegOnSeparateAxis(&cross, &extentB[1], &extentB[2]);
			else if (j == 1) rB = LenSegOnSeparateAxis(&cross, &extentB[0], &extentB[2]);
			else			 rB = LenSegOnSeparateAxis(&cross, &extentB[0], &extentB[1]);

			L = fabs(centerInterval.Dot(cross));
			
			if (L > rA + rB) return false;//衝突していない
		}
	}

	return true;		//衝突している (分離平面が存在しない)
}


/**
 * @brief OBBと球の当たり判定
 *
 * @param[in] obb	 判定対象の直方体
 * @param[in] sphere 判定対象の球
 *
 * @retval true  衝突している場合
 * @retval false 衝突していない場合
 */
bool IsHit(const OBBCollider& obb, const SphereCollider& sphere)
{
	SimpleMath::Vector3 vec{ 0.0f,0.0f,0.0f };
	SimpleMath::Vector3 delta = sphere.GetCenter() - obb.GetCenter();

	//各軸についてはみ出た部分のベクトルを算出
	for (int i = 0; i < 3; i++)
	{
		float L = obb.GetHalfLength(i);

		if (L <= 0) continue;

		float s = delta.Dot(obb.GetAxes(i)) / L;	//投影距離/半径

		s = fabs(s);

		if (s > 1.0f)
		{
			vec += (1.0f - s) * L * obb.GetAxes(i);
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
 * @brief 球と球の距離の算出
 *
 * @param[in] sphereA 判定対象の球A
 * @param[in] sphereB 判定対象の球B
 *
 * @return 球と球の距離
 */
float Distance(const SphereCollider& sphereA, const SphereCollider& sphereB)
{
	float x = sphereB.GetCenter().x - sphereA.GetCenter().x;
	float y = sphereB.GetCenter().y - sphereA.GetCenter().y;
	float z = sphereB.GetCenter().z - sphereA.GetCenter().z;

	return sqrtf(x * x + y * y + z * z);
}

MTV CalculateMTV(const SphereCollider& sphereA, const SphereCollider& sphereB)
{
	MTV mtv;

	SimpleMath::Vector3 direction = sphereB.GetCenter() - sphereA.GetCenter();
	mtv.direction = direction;

	float centerDistance = mtv.direction.Length();
	float radiusSum = sphereA.GetRadius() + sphereB.GetRadius();
	mtv.distance = radiusSum - centerDistance;

	return mtv;
}


/**
 * @brief OBBとOBBの最短距離の算出
 *
 * @param[in] obbA 判定対象の直方体A
 * @param[in] obbB 判定対象の直方体B
 *
 * @return OBBとOBBの最短距離
 */
MTV CalculateMTV(const OBBCollider& obbA, const OBBCollider& obbB)
{
	MTV mtv;

	return mtv;
}


/**
 * @brief OBBと球の最短距離
 *
 * @param[in] obb	 判定対象の直方体
 * @param[in] sphere 判定対象の球
 *
 * @return OBBと球の最短距離
 */
MTV CalculateMTV(const OBBCollider& obb, const SphereCollider& sphere)
{
	MTV mtv = { SimpleMath::Vector3::Zero ,FLT_MAX };

	//球の中心からOBB上の最近傍点を求める
	SimpleMath::Vector3 delta = sphere.GetCenter() - obb.GetCenter();
	SimpleMath::Vector3 closest = obb.GetCenter();

	for (int i = 0; i < 3; i++)
	{
		float dist = delta.Dot(obb.GetAxes(i));
		float clamped = std::max(-obb.GetHalfLength(i), std::min(dist, obb.GetHalfLength(i)));
		closest += obb.GetAxes(i) * clamped;
	}

	//最近傍点と球の中心との距離を求める
	SimpleMath::Vector3 displacement = sphere.GetCenter() - closest;
	float distance = displacement.Length();
	float penetration = sphere.GetRadius() - distance;

	if (penetration > 0.0f && distance > 0.0001f)
	{
		//押し出しベクトルを計算
		mtv.direction = displacement / distance;	//正規化
		mtv.distance = penetration;
	}
	else
	{
		mtv.direction = SimpleMath::Vector3::Zero;
		mtv.distance = 0.0f;
	}

	return mtv;
}

//平面と球の距離の算出
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

float CalculateProjectionRadius(DirectX::SimpleMath::Vector3 axisA, 
								DirectX::SimpleMath::Vector3 extentA, 
								DirectX::SimpleMath::Vector3 extentB[3])
{
	float rA = extentA.Length();
	float rB = LenSegOnSeparateAxis(&axisA, &extentB[0], &extentB[1], &extentB[2]);

	return rA + rB;
}


// 分離軸に投影された軸成分から投影線分長を算出
float LenSegOnSeparateAxis(SimpleMath::Vector3* sep, 
						   SimpleMath::Vector3* e1,
						   SimpleMath::Vector3* e2,
						   SimpleMath::Vector3* e3)
{
	//3つの内積の絶対値の和で投影線分長を計算
	//分離軸Sepは標準化されていること
	float r1 = fabs(sep->Dot(*e1));
	float r2 = fabs(sep->Dot(*e2));
	float r3 = e3 ? (fabs(sep->Dot(*e3))) : 0.0f;

	return r1 + r2 + r3;
}

//法線ベクトルの算出
DirectX::SimpleMath::Plane CalculatePlane(const DirectX::SimpleMath::Vector3& p1,
										  const DirectX::SimpleMath::Vector3& p2, 
										  const DirectX::SimpleMath::Vector3& p3)
{
	SimpleMath::Vector3 v1 = p2 - p1;
	SimpleMath::Vector3 v2 = p3 - p1;
	SimpleMath::Vector3 normal = v1.Cross(v2);	//外積を求める
	normal.Normalize();							//正規化

	float D = -(normal.Dot(p1));

	return SimpleMath::Plane(normal.x, normal.y, normal.z, D);
}

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
