#pragma once
#include <vector>

class Trigger
{
public:
	//Depreciated use OnEnter and OnExit instead
	virtual void OnTrigger();

	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnHold();

	void SetTriggerEntity(int triggerEnt);
	int GetTriggerEntity();

	void AddTargetEntity(int entity);
	void SetTargetEntities(std::vector<int> entities);
	std::vector<int> GetTargetEntities();
	bool hold = true;
protected:
	int m_triggerEntity;
	std::vector<int> m_targetEntities;
};

