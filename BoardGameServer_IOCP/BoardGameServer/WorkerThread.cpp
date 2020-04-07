#include "WorkerThread.h"
#include "User.h"

WorkerThread::WorkerThread(HANDLE& _iocpHandle)
{
	m_hIocpHandle = _iocpHandle;
}

WorkerThread::~WorkerThread()
{
	
}

bool WorkerThread::Init()
{
	ThreadClass<WorkerThread>::Start(this);

	return true;
}

void WorkerThread::LoopRun()
{
	DWORD bytes;
	Session* pSession = nullptr;
	ST_OVERLAPPED* overlapped;

	while (1)
	{
		if (GetQueuedCompletionStatus(
			m_hIocpHandle,
			&bytes,
			(PULONG_PTR)&pSession,
			(LPOVERLAPPED*)&overlapped,
			INFINITE) == 0)
		{
			printf("[INFO] GetQueuedCompletionStatus Failure\n");
		}
		else
		{
			overlapped->session->CheckCompletion(overlapped, bytes);
		}
	}
}