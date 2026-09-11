#pragma once
struct Status
{
	int	MaxHp;
	int NowHp;
	int AttackPower;

	void Init(int maxhp, int attackpower)
	{
		MaxHp = maxhp;
		NowHp = maxhp;
		AttackPower = attackpower;
	}

	void AddDamage(int damage)
	{
		NowHp -= damage;
		if (NowHp < 0)
		{
			NowHp = 0;
		}
	}

	bool IsAlive()
	{
		if (NowHp == 0)
		{
			return false;
		}
		else 
		{
			return true;
		}
	}
};