//#include "ThreadPool.h"
//
//ThreadPool::ThreadPool(int threadNum_) : threadNum(threadNum_), stop_all(false) {
//	vWorkerThread.reserve(threadNum);
//	for (size_t i = 0; i < threadNum; ++i) {
//		vWorkerThread.emplace_back([this]() { this->WorkerThread(); });
//	}
//}
//
//
//ThreadPool::~ThreadPool()
//{
//}
//
//void ThreadPool::EnqueueJob(function<void()> job)
//{
//
//}