#include "NewRingBuffer.h"

NewRingBuffer::NewRingBuffer()
{
	
}

NewRingBuffer::~NewRingBuffer()
{

}

void NewRingBuffer::Init(int _size, int _tempSize)
{
	bufferMaxSize = _size;
	tempBufferSize = _tempSize;

	buffer = new char[_size];

	bufferStartPoint = buffer + tempBufferSize;
	writePoint = readPoint = bufferStartPoint;
}

void NewRingBuffer::Reset()
{
	//버퍼를 날리고
	//SAFE_DELETE_ARRAY(buffer);

	//버퍼와 WP, RP 초기화
	buffer = bufferStartPoint - tempBufferSize;
	writePoint = readPoint = bufferStartPoint;
}

void NewRingBuffer::Write(int _size)
{
	writePoint += _size;

	if (writePoint >= buffer + bufferMaxSize)
	{
		writePoint = bufferStartPoint;
	}
}

void NewRingBuffer::Read(int _size)
{
	readPoint += _size;

	if (readPoint >= buffer + bufferMaxSize)
	{
		readPoint = bufferStartPoint;
	}
}

bool NewRingBuffer::CanParsing()
{
	DWORD dataInBuffer = GetDataInBuffer();

	//링버퍼 안에 데이터가 2이상 있는가? (패킷 사이즈 읽을 수 있는 최소한의 크기 = 2)
	if (dataInBuffer >= 2)
	{
		return true;
	}

	//데이터가 없거나 1들어와서 크기조차 알 수 없을 경우
	return false;
}

char* NewRingBuffer::Parsing()
{
	//버퍼의 끝지점부터 읽는 지점까지의 사이즈
	DWORD remainedSize = (buffer + bufferMaxSize) - readPoint;
	//파싱할 데이터는 읽는 지점부터 읽어온다.
	char* parsingData = readPoint;

	//버퍼의 끝지점부터 읽는 지점까지 사이즈가 1밖에 없는 경우 -> 이어줘야한다.
	if (remainedSize == 1)
	{
		char* tempBuffer = bufferStartPoint - 1;

		*tempBuffer = *readPoint;

		locker.EnterLock();
		parsingData = readPoint = tempBuffer;
		locker.LeaveLock();

		readPoint += GetPacketSize();

		return parsingData;
	}
	//버퍼의 끝지점부터 읽는 지점이 같다 = 버퍼의 첫지점으로 읽는 지점을 옮겨야한다.
	else if (remainedSize == 0)
	{
		locker.EnterLock();
		readPoint = bufferStartPoint;
		locker.LeaveLock();
	}

	unsigned short packetSize = GetPacketSize();

	if (packetSize > remainedSize)
	{
		memcpy(bufferStartPoint - remainedSize, readPoint, remainedSize);
		parsingData = readPoint = (bufferStartPoint - remainedSize);
	}

	readPoint += packetSize;
	return parsingData;
}

//readPoint와 writePoint가 같을 때 처리
//동기화 문제
//		-> Parsing쪽에서 ReadPoint가 옮겨질때 Lock을 걸어줬다(191202)

DWORD NewRingBuffer::GetWriteableSize()
{
	if (readPoint < writePoint)
	{
		return (buffer + bufferMaxSize) - writePoint;
	}
	else if (readPoint == writePoint)
	{
		//readPoint와 writePoint가 같은데
		//데이터가 있는 경우 == 한바퀴 돌아서 같아짐
		if (this->GetDataInBuffer() > 0)
		{
			return 0;
		}
		//readPoint와 writePoint가 같은데
		//데이터가 없는 경우 == ringBuffer가 데이터를 받는 시점이거나 처리할게 없음(문제X)
		else
		{
			return (buffer + bufferMaxSize) - writePoint;
		}
	}
	else
	{
		return readPoint - writePoint;
	}
}

DWORD NewRingBuffer::GetReadableSize()
{
	if (readPoint <= writePoint)
	{
		return writePoint - readPoint;
	}
	else
	{
		return (buffer + bufferMaxSize) - readPoint;
	}
}

DWORD NewRingBuffer::GetDataInBuffer()
{
	if (readPoint > writePoint)
	{
		return (buffer + bufferMaxSize) - readPoint + (writePoint - bufferStartPoint);
	}
	else
	{
		return (int)(writePoint - readPoint);
	}
}

unsigned short NewRingBuffer::GetPacketSize()
{
	return *(unsigned short*)readPoint;
}