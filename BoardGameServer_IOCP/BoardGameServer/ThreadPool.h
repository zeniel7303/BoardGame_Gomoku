//#pragma once
//
////참고
////https://modoocode.com/285
//
//#include <functional>
//#include <queue>
//#include <thread>
//#include <vector>
//
//using namespace std;
//
//class ThreadPool
//{
//private:
//	int threadNum;
//
//	vector<thread> vWorkerThread;
//	queue<function<void()>> jobs;
//
//	// 모든 쓰레드 종료
//	bool stop_all;
//
//	// Worker 쓰레드
//	void WorkerThread();
//
//public:
//	ThreadPool(int threadNum_);
//	~ThreadPool();
//
//	// job 을 추가한다.
//	void EnqueueJob(function<void()> job);
//};
//
