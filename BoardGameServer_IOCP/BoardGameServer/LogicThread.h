#pragma once
#include "ThreadClass.h"

class LogicThread : public ThreadClass<LogicThread>
{
private:


public:
	LogicThread();
	~LogicThread();

	void Init();
	void LoopRun();
};

