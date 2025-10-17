#pragma once

// UŒ‚‚Ìí—Ş
enum class AttackType
{
	BASIC,		// ’Êí	
	ROLLING,	// “]‚ª‚è
	HEAVY,		// d—Ê

	TYPE_NUM	// í—Ş”
};

struct AttackData
{
	AttackType type;
	float force;
};

inline AttackType& operator ++ (AttackType& type)
{
	type = static_cast<AttackType>(static_cast<int>(type) + 1);
	if (type == AttackType::TYPE_NUM) type = AttackType::BASIC;

	return type;
}

inline AttackType& operator -- (AttackType& type)
{
	type = static_cast<AttackType>(static_cast<int>(type) - 1);
	if (type < AttackType::BASIC) type = AttackType::HEAVY;

	return type;
}
