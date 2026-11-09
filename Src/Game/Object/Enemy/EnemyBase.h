#pragma once
#include"../ActorBase/Actorbase.h"
#include"../../System/Element.h"

class EnemyBase : public ActorBase
{
private:

	ElementType m_element;
	int m_elementCnt;

public:
	EnemyBase();
	~EnemyBase();

	void Init();

	void AddElement(ElementType);

	ElementType GetElement()const;

	virtual void Pull(const VECTOR& center, float power);

protected:

	void StepElement();

	unsigned int GetElementColor();

};