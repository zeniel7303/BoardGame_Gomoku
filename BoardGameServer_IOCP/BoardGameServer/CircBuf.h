#pragma once
#include <iostream>

//√‚√≥ :
//https://gist.github.com/xstherrera1987/3196485

/*
   T must implement operator=, copy ctor
*/

template<typename T> class CircBuf {
	// don't use default ctor
	CircBuf();

	const int size;
	T *data;
	bool *canUse;
	bool *isUsed;
	int front;
	int end;
	int count;
public:
	CircBuf(int);
	~CircBuf();

	bool IsEmpty();
	bool IsFull();
	void Add(const T&, bool boolValue);
	void Add(const T&, int num, bool boolValue);
	T* Remove();

	int getSize() { return size; }
	int getCount() { return count; }
	int getFront() { return front; }
	int getEnd() { return end; }

	bool boolCanUse(int num) { return canUse[num]; }
	bool getIsUsed(int num) { return isUsed[num]; }
	void setIsUsed(int num, bool value) { isUsed[num] = value; }
	T* getBuffer(int num) { return &data[num]; }

	T* getFrontBuffer() { return &data[front]; }
	T* getEndBuffer() { return &data[end]; }
};

template<typename T> CircBuf<T>::CircBuf(int sz) : size(sz) {
	if (sz == 0) throw std::invalid_argument("size cannot be zero");
	data = new T[sz];
	canUse = new bool[sz];
	isUsed = new bool[sz];
	front = 0;
	end = 0;
	count = 0;
}
template<typename T> CircBuf<T>::~CircBuf() {
	delete data;
}

template<typename T> bool CircBuf<T>::IsEmpty()
{
	if (front == end)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<typename T> bool CircBuf<T>::IsFull()
{
	int temp = (end + 1) % size;

	if (temp == front)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<typename T> void CircBuf<T>::Add(const T &t, bool boolValue) {
	if (IsFull()) {
		Remove();
		Add(t, boolValue);
	}
	else {
		count++;
		end = (end + 1) % size;
		data[end] = t;
		canUse[end] = boolValue;
		isUsed[end] = false;
	}
}

template<typename T> void CircBuf<T>::Add(const T &t, int num, bool boolValue) {
	data[num] = t;
	canUse[num] = boolValue;
}

template<typename T> T* CircBuf<T>::Remove() {
	if (!IsEmpty()) {
		count--;

		front = (front + 1) % size;
		//*t = data[front];
		return &data[front];
	}
}

/*
// returns true if add was successful, false if the buffer is already full
template<typename T> bool CircBuf<T>::add(const T &t) {
	if (full()) {
		return false;
	}
	else {
		// find index where insert will occur
		int end = (front + count) % size;
		data[end] = t;
		count++;
		return true;
	}
}

// returns true if add was successful, false if the buffer is already full
template<typename T> bool CircBuf<T>::add(const T &t, int num) {
	if (data[num] == NULL) {
		return false;
	}
	else {
		// find index where insert will occur
		data[num] = t;
		return true;
	}
}

// returns true if there is something to remove, false otherwise
template<typename T> bool CircBuf<T>::remove(T *t) {
	if (empty()) {
		return false;
	}
	else {
		*t = data[front];

		front = (front == size - 1) ? 0 : front + 1;
		count--;
		return true;
	}
}
*/