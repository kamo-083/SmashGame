#pragma once

enum class WeaponType
{
	BASIC,
	ROLLING,
	HEAVY,
	TYPE_NUM
};

struct Weapon
{
	WeaponType type;
	float force;
};

inline WeaponType& operator ++ (WeaponType& type)
{
	type = static_cast<WeaponType>(static_cast<int>(type) + 1);
	if (type == WeaponType::TYPE_NUM) type = WeaponType::BASIC;

	return type;
}

inline WeaponType& operator -- (WeaponType& type)
{
	type = static_cast<WeaponType>(static_cast<int>(type) - 1);
	if (type < WeaponType::BASIC) type = WeaponType::HEAVY;

	return type;
}
