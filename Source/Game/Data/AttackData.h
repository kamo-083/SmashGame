// 多重インクルードの防止 =====================================================
#pragma once

// 攻撃の種類
enum class AttackType
{
	BASIC,		// 通常	
	ROLLING,	// 転がり
	HEAVY,		// 重量

	TYPE_NUM	// 種類数
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