#pragma once
#include <stack>

#include "CriticalSectionClass.h"

using namespace std;

//=====================================================

//ObjectPool

//=====================================================

template<typename T>
class ObjectPool
{
private:
	CriticalSectionClass locker;
	stack<T*> objects;

	// 최대 배열 크기
	int maxSize; 

public:
	int GetCount()
	{
		return objects.size();
	}

	// 사이즈만큼 오브젝트를 만든다.
	ObjectPool(int size = 100) 
	{
		maxSize = size;

		for (int i = 0; i < maxSize; ++i) 
		{
			T* newObject = new T();
			objects.push(newObject);
		}
	}

	// 오브젝트를 비운다.
	~ObjectPool()
	{
		locker.EnterLock();

		while (!objects.empty()) 
		{
			T* object = objects.top();
			objects.pop();
			delete object;
		}

		maxSize = 0;

		locker.LeaveLock();
	}

	// 오브젝트를 꺼낸다.
	T* PopObject()
	{
		locker.EnterLock();

		// 오브젝트가 없다면 확장한다.
		if (objects.empty()) 
		{
			Expand();
		}

		T* retVal = objects.top();
		objects.pop();

		locker.LeaveLock();

		return retVal;
	}

	// 현재 크기만큼 새로운 오브젝트를 넣어 확장한다.
	void Expand() 
	{
		locker.EnterLock();

		// 늘린 크기만큼 새로운 오브젝트를 넣어준다.
		for (int i = 0; i < maxSize; ++i)
		{
			T* newObject = new T();
			objects.push(newObject);
		}

		// 최대 크기를 늘린다.
		maxSize += maxSize;

		locker.LeaveLock();
	}

	// 오브젝트를 수거한다.
	void ReturnObject(T* _object)
	{
		//locker.EnterLock();

		objects.push(_object);

		//locker.LeaveLock();
	}
};