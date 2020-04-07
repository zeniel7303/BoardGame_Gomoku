#pragma once
#include <algorithm>

#include "ObjectPool.h"
#include "CriticalSectionClass.h"

//=====================================================

//SessionManager

//Session들을 관리하기위한 매니저

//ObjectPool을 상속받아 서버 구동시 미리 User를 100개를 만들어둔다.

//CreateSession은 Accept가 현재 동기이므로 동시에 여러번 접근하지 않으므로 CriticalSection을 사용하지 않았으나
//DeleteSession에서는 동시에 여러번 Session을 제거하게 접근할 수 있으므로 CriticalSection으로 막아두었다.	

//=====================================================

template <typename T>
class SessionManager : public ObjectPool<T>
{
private:
	int						m_count;

	vector<T*>				m_vSessionList;

	CriticalSectionClass	m_locker;

public:
	SessionManager()
	{	
	}

	~SessionManager()
	{
	}

	void Init()
	{
		m_count = 0;
	}

	T* CreateSession()
	{
		m_count++;

		return ObjectPool<T>::PopObject();
	}

	void DeleteSession(T* _t)
	{
		m_locker.EnterLock();

		typename vector<T*>::iterator iter = 
			find(m_vSessionList.begin(), m_vSessionList.end(), _t);
		m_vSessionList.erase(iter);

		ObjectPool<T>::ReturnObject(_t);

		m_locker.LeaveLock();
	}

	void AddSessionList(T* _t)
	{
		m_vSessionList.push_back(_t);
	}

	vector<T*> GetSessionList() { return m_vSessionList; }
	int GetCount() { return m_count; }
};