#pragma once
//https://jinheeahn.wordpress.com/2015/01/29/ringbuffer%EB%9E%80-%EC%99%9C-%EC%82%AC%EC%9A%A9%ED%95%98%EB%8A%94%EA%B0%80/

//https://phiru.tistory.com/104
#include "packet.h"

#pragma warning(disable:4996)

using namespace std;

//NewRingBuffer로 새로짬 참고

class RingBuffer
{
private:
	//버퍼 사이즈
	int			bufferSize;
	//임시 저장 버퍼 사이즈
	int			tempBufferSize;
	//버퍼 주소
	char*		buffer;
	//읽는 지점
	char*		readPoint;
	//쓰는 지점
	char*		writePoint;
	//버퍼의 시작 지점
	char*		bufferStartPoint;
	//버퍼의 끝 지점
	char*		bufferEndPoint;
	//버퍼 안 데이터 크기
	int			dataInBuffer;

public:
	//생성자
	RingBuffer()
	{
		buffer = nullptr;
		readPoint = nullptr;
		writePoint = nullptr;
		bufferStartPoint = nullptr;
		bufferEndPoint = nullptr;
		bufferSize = 0;
		tempBufferSize = 0;
		dataInBuffer = 0;
	}
	~RingBuffer()
	{

	}

	//_size만큼 링버퍼 생성
	void Init(int _size)
	{
		buffer = new char[_size];
		if (buffer == nullptr) return;

		bufferSize = _size;

		this->Reset();
	}

	//리셋
	void Reset()
	{
		bufferStartPoint = buffer;
		bufferEndPoint = buffer + bufferSize;
		readPoint = buffer;
		writePoint = buffer;

		dataInBuffer = 0;
	}

	//wrirtePoint _size만큼 이동
	void Write(int _size)
	{
		writePoint += _size;
		dataInBuffer += _size;

		if (writePoint == bufferEndPoint)
		{
			writePoint = buffer;
		}
	}

	//읽은 _size 만큼 readPoint 이동
	void Read(int _size)
	{
		readPoint += _size;

		if (readPoint >= bufferEndPoint)
		{
			readPoint = buffer + (readPoint - bufferEndPoint);
		}

		dataInBuffer -= _size;
	}

	//링버퍼 내부의 데이터 크기
	int GetDataInBuffer()
	{
		return dataInBuffer;
	}

	//write 가능한 크기
	int GetWriteSize()
	{
		if (readPoint <= writePoint)
		{
			return (int)(bufferEndPoint - writePoint);
		}
		else
		{
			return (int)(readPoint - writePoint);
		}
	}

	//Read 가능한 크기
	int GetReadSize()
	{
		if (readPoint <= writePoint)
		{
			return (int)(writePoint - readPoint);
		}
		else
		{
			return (int)(bufferEndPoint - readPoint);
		}
	}

	//writePoint 반환
	char* GetWritePoint()
	{
		return writePoint;
	}

	//readPoint 반환
	char* GetReadPoint()
	{
		return readPoint;
	}

	//		bufferEndPoint - _buffer 값
	int GetRemainedSize(char* _buffer) 
	{ 
		return (int)(bufferEndPoint - _buffer); 
	}

	int GetBufferSize()
	{
		return bufferSize;
	}
};