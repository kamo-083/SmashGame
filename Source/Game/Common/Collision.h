/**
 * @file   Collision.h
 *
 * @brief  球・OBBの当たり判定に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// 最小押し出しベクトル
struct MTV
{
	DirectX::SimpleMath::Vector3 direction;	// 方向
	float distance;							// 距離
};


// 球の当たり判定
class SphereCollider
{
private:
	DirectX::SimpleMath::Vector3 m_center;	//中心座標
	float m_radius;							//半径

public:
	//コンストラクタ・デストラクタ
	SphereCollider();
	SphereCollider(DirectX::SimpleMath::Vector3 center,float radius);
	~SphereCollider() = default;

	//取得
	DirectX::SimpleMath::Vector3 GetCenter() const { return m_center; }	//中心座標の取得
	float GetRadius() const { return m_radius; }						//半径の取得

	//設定
	void SetCenter(DirectX::SimpleMath::Vector3 center) { m_center = center; }	//中心位置の設定
	void SetRadius(float radius) { m_radius = radius; }							//半径の設定
};

// OBBの当たり判定
class OBBCollider
{
public:
	struct OBB
	{
		DirectX::SimpleMath::Vector3 center;		//中心座標
		DirectX::SimpleMath::Vector3 axis[3];		//方向ベクトル
		DirectX::SimpleMath::Vector3 halfLength;	//中心座標から面までの長さ
		DirectX::SimpleMath::Quaternion rotation;	//回転
	};

	// コライダーの種類(主にステージの地形に使用)
	enum class CollisionType
	{
		Ground,	// 地面
		Wall,	// 壁
		Slope,	// 坂
		Others,	// その他
	};

private:
	OBB m_obb;
	MTV m_mtv;

public:
	//コンストラクタ・デストラクタ
	OBBCollider();
	OBBCollider(DirectX::SimpleMath::Vector3 center,
				DirectX::SimpleMath::Quaternion rotation,
				DirectX::SimpleMath::Vector3 halfLength);
	~OBBCollider() = default;

	//取得
	DirectX::SimpleMath::Vector3 GetCenter() const { return m_obb.center; }			//中心座標の取得
	float GetCenter(int n) const;
	DirectX::SimpleMath::Vector3 GetHalfLength() const { return m_obb.halfLength; }	//中心座標から面までの長さの取得
	float GetHalfLength(int n) const;
	DirectX::SimpleMath::Vector3* GetAxis() { return m_obb.axis; }					//方向ベクトルの取得
	DirectX::SimpleMath::Vector3 GetAxis(int n) const { return m_obb.axis[n]; }
	DirectX::SimpleMath::Quaternion GetRotation() const { return m_obb.rotation; }	//回転の取得

	//設定
	void SetCenter(DirectX::SimpleMath::Vector3 center) { m_obb.center = center; }					//中心位置の設定
	void SetHalfLength(DirectX::SimpleMath::Vector3 halfLength) { m_obb.halfLength = halfLength; }  //中心座標から面までの長さの設定
	void SetRotation(DirectX::SimpleMath::Quaternion rotation);										//回転の設定
};

//球と球の当たり判定
bool IsHit(const SphereCollider& sphereA, const SphereCollider& sphereB);

//OBBとOBBの当たり判定
bool IsHit(const OBBCollider& obbA, const OBBCollider& obbB);

//OBBと球の当たり判定
bool IsHit(const OBBCollider& obb, const SphereCollider& sphere);

//球と球の最短距離の算出
float Distance(const SphereCollider& sphereA, const SphereCollider& sphereB);

//球と球のMTVの算出
MTV CalculateMTV(const SphereCollider& sphereA, const SphereCollider& sphereB);

//OBBとOBBのMTVの算出
MTV CalculateMTV(const OBBCollider& obbA, const OBBCollider& obbB);

//OBBと球のMTVの算出
MTV CalculateMTV(const OBBCollider& obb, const SphereCollider& sphere);

//平面と球の最短距離の算出
float Distance(
	const DirectX::SimpleMath::Plane& plane,
	const DirectX::SimpleMath::Vector3& center);

// 指定された軸で分離軸を判定
bool TryAxis(
	const DirectX::SimpleMath::Vector3& axisRaw,
	const DirectX::SimpleMath::Vector3& centerInterval,
	const DirectX::SimpleMath::Vector3 extentA[3],
	const DirectX::SimpleMath::Vector3 extentB[3],
	bool useMTV,
	float& minOverlap,
	DirectX::SimpleMath::Vector3& bestAxis);

// 指定された軸上での2つのOBBの投影半径を計算
float CalculateProjectionRadius(
	DirectX::SimpleMath::Vector3 axisA,
	DirectX::SimpleMath::Vector3 extentA,
	DirectX::SimpleMath::Vector3 extentB[3]);

// 分離軸に対するOBBの投影半径を算出
float LenSegOnSeparateAxis(
	DirectX::SimpleMath::Vector3* sep,
	DirectX::SimpleMath::Vector3* e1,
	DirectX::SimpleMath::Vector3* e2,
	DirectX::SimpleMath::Vector3* e3 = 0);

// 3点から平面を算出する
DirectX::SimpleMath::Plane CalculatePlane(
	const DirectX::SimpleMath::Vector3& p1,
	const DirectX::SimpleMath::Vector3& p2,
	const DirectX::SimpleMath::Vector3& p3);

// クォータニオンから各軸ベクトルを算出
void AxisFromQuaternion(
	const DirectX::SimpleMath::Quaternion& rotation,
	DirectX::SimpleMath::Vector3* axis);

// 衝突面の法線から衝突面の種類を判定
OBBCollider::CollisionType DetermineCollisionType(const DirectX::SimpleMath::Vector3& normal);