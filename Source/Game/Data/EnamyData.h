#pragma once

enum class EnemyType
{
	Basic,
	Heavy,
	Light,
};

struct EnemyDesc
{
	EnemyType type;
	float radius;
	float speed;
	float mass;
	float max_speed;
};
