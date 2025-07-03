#pragma once

struct MTV
{
	DirectX::SimpleMath::Vector3 direction;
	float distance;
};


class SphereCollider
{
private:
	DirectX::SimpleMath::Vector3 m_center;	//中心座標
	float m_radius;							//半径

public:
	//コンストラクタ・デストラクタ
	SphereCollider();
	SphereCollider(DirectX::SimpleMath::Vector3 center,float radius);
	~SphereCollider();

	//取得
	DirectX::SimpleMath::Vector3 GetCenter() const;	//中心座標の取得
	float GetRadius() const;						//半径の取得

	//設定
	void SetCenter(DirectX::SimpleMath::Vector3 center);	//中心座標の設定
	void SetRadius(float radius);							//半径の設定
};

class OBBCollider
{
public:
	struct OBB
	{
		DirectX::SimpleMath::Vector3 center;		//中心座標
		DirectX::SimpleMath::Vector3 axis[3];		//方向ベクトル
		DirectX::SimpleMath::Vector3 halfLength;	//中心座標から面までの長さ
	};

	enum class CollisionType
	{
		Ground,
		Wall,
		Slope,
		Others,
	};

private:
	OBB m_obb;
	MTV m_mtv;

public:
	//コンストラクタ・デストラクタ
	OBBCollider();
	OBBCollider(DirectX::SimpleMath::Vector3 center, 
				DirectX::SimpleMath::Vector3 axis[3],
				DirectX::SimpleMath::Vector3 halfLength);
	~OBBCollider();

	//取得
	DirectX::SimpleMath::Vector3 GetCenter() const;		//中心座標の取得
	float GetCenter(int n) const;
	DirectX::SimpleMath::Vector3 GetAxes(int n) const;	//方向ベクトルの取得
	DirectX::SimpleMath::Vector3 GetHalfLength() const;	//中心座標から面までの長さの取得
	float GetHalfLength(int n) const;

	//設定
	void SetCenter(DirectX::SimpleMath::Vector3 center);		 //中心座標の設定
	void SetAxes(DirectX::SimpleMath::Vector3 axis[3]);			 //方向ベクトルの設定
	void SetAxes(DirectX::SimpleMath::Vector3 axis);			 //方向ベクトルの設定
	void SetHalfLength(DirectX::SimpleMath::Vector3 halfLength); //中心座標から面までの長さの設定
};

//球と球の衝突判定
bool IsHit(const SphereCollider& sphereA, const SphereCollider& sphereB);

//OBBとOBB同士の衝突判定
bool IsHit(const OBBCollider& obbA, const OBBCollider& obbB);

//OBBと球の衝突判定
bool IsHit(const OBBCollider& obb, const SphereCollider& sphere);

//球と球の距離の算出
float Distance(const SphereCollider& sphereA, const SphereCollider& sphereB);

//球と球の最短距離の算出
MTV CalculateMTV(const SphereCollider& sphereA, const SphereCollider& sphereB);

//OBBとOBBの最短距離の算出
MTV CalculateMTV(const OBBCollider& obbA, const OBBCollider& obbB);

//OBBと球の最短距離の算出
MTV CalculateMTV(const OBBCollider& obb, const SphereCollider& sphere);

//平面と球の距離の算出
float Distance(const DirectX::SimpleMath::Plane& plane,
			   const DirectX::SimpleMath::Vector3& center);

//分離軸の計算
float CalculateProjectionRadius(DirectX::SimpleMath::Vector3 axisA,
								DirectX::SimpleMath::Vector3 extentA,
								DirectX::SimpleMath::Vector3 extentB[3]);

// 分離軸に投影された軸成分から投影線分長を算出
float LenSegOnSeparateAxis(DirectX::SimpleMath::Vector3* sep,
						   DirectX::SimpleMath::Vector3* e1,
						   DirectX::SimpleMath::Vector3* e2,
						   DirectX::SimpleMath::Vector3* e3 = 0 );

//法線ベクトルの算出
DirectX::SimpleMath::Plane CalculatePlane(const DirectX::SimpleMath::Vector3& p1,
										  const DirectX::SimpleMath::Vector3& p2,
										  const DirectX::SimpleMath::Vector3& p3);

//衝突したオブジェクトが地面か壁かの判定
OBBCollider::CollisionType DetermineCollisionType(const DirectX::SimpleMath::Vector3& normal);