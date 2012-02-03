#include "pch.h"
#include "realtimereader.h"
#include "exception.h"

RealTimeReader::RealTimeReader()
	: mFile(INVALID_HANDLE_VALUE), mBytesPerSec(1000), mMaxSeconds(5),
	mBufferCursor(0), mLoseOneByte(false)
{
	mMaxBufferSize = mBytesPerSec * mMaxSeconds;
	mpStreamShort = new std::ofstream();
	mpStreamChar = new std::ofstream();
}

RealTimeReader::~RealTimeReader()
{
	close();
	delete mpStreamShort;
	delete mpStreamChar;
}

void RealTimeReader::open(const QString& port, int baudRate)
{
	mFile = CreateFileA(port.toAscii().data(), // port name
						GENERIC_READ, // Desired access
						0, // shared mode
						NULL, // security
						OPEN_EXISTING, // creation disposition
						0,
						NULL); // non overlapped

	if(mFile == INVALID_HANDLE_VALUE)
		throw_Exception("Can't open port COM!");

	// -----------------------
	// configuring serial port settings

	DCB dcbConfig = {0};
	if(!GetCommState(mFile, &dcbConfig))
	{
		CloseHandle(mFile);
		throw_Exception("Can't get port config!");
	}

	// http://msdn.microsoft.com/en-us/library/aa363214(VS.85).aspx
	dcbConfig.BaudRate = baudRate;
	dcbConfig.fParity = 0;
	dcbConfig.fOutxCtsFlow = 0;
	dcbConfig.fOutxDsrFlow = 1;
	dcbConfig.fDsrSensitivity = 0;
	dcbConfig.fDtrControl = DTR_CONTROL_DISABLE;
	dcbConfig.fOutX = 0;
	dcbConfig.fInX = 0;
	dcbConfig.fRtsControl = RTS_CONTROL_DISABLE;
	dcbConfig.fAbortOnError = 0;
	dcbConfig.ByteSize = 8;
	dcbConfig.Parity = NOPARITY;
	dcbConfig.StopBits = ONESTOPBIT;

	if(!SetCommState(mFile, &dcbConfig))
	{
		CloseHandle(mFile);
		throw_Exception("Can't set port config!")
	}
	COMMTIMEOUTS timeouts = {0};

	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;

	if(!SetCommTimeouts(mFile, &timeouts))
	{
		CloseHandle(mFile);
		throw_Exception("Can't set timeouts!")
	}

	// alokuj bufor
	mDataBuffer.resize(mMaxBufferSize);
	for(int i = 0; i < mDataBuffer.size(); ++i)
		mDataBuffer[i] = 0;

	// otworz plik do zapisu danych
	mpStreamShort->open("dataout_short.txt", std::ios::out);
	mpStreamChar->open("dataout_char.txt", std::ios::out);
}

void RealTimeReader::close()
{
	if(mFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(mFile);
		mFile = INVALID_HANDLE_VALUE;

		// reset pol
		mBytesPerSec = 0;
		mMaxSeconds = 0;
		mMaxBufferSize = 0;
		mBufferCursor = 0;
	}
}

void RealTimeReader::setReadingParams(int bytesPerSec, int maxSeconds)
{
	mMaxSeconds = maxSeconds;
	mBytesPerSec = bytesPerSec;
	mMaxBufferSize = maxSeconds * bytesPerSec;
}

void RealTimeReader::read()
{
	DWORD dwEventMask; // nieuzywane
	mTempBuffer.clear();

	// Ustaw jakie zdarzenia portu monitorowac (odebrano znak)
	if(!SetCommMask(mFile, EV_RXCHAR))
		throw_Exception("Can't specify a set of events to be monitored.");

	// Czekaj na zdarzenie (odebrano znak)
	if(WaitCommEvent(mFile, &dwEventMask, NULL))
	{
		unsigned char ch[1024];
		DWORD dwReadSize;

		do
		{
			if(ReadFile(mFile, ch, 1024, &dwReadSize, NULL) != 0)
			{
				if(dwReadSize > 0)
				{
					mTempBuffer.append(reinterpret_cast<const char*>(ch), dwReadSize);
				}
			}
			else
			{
				break;
			}
		} while(dwReadSize > 0);

		// czy trzeba zapetlic bufor?
		int rest = (mBufferCursor + mTempBuffer.size()) - mMaxBufferSize;

		// dotarlismy do konca bufora, trzeba go zapetlic
		if(rest > 0)
		{
			// skopiuj dane z tymczasowego bufora
			for(int i = 0; i < mTempBuffer.size() - rest; ++i)
			{
				mDataBuffer[i + mBufferCursor] = mTempBuffer[i];
			}

			// jesli mamy zgubic jeden bajt
			if(mLoseOneByte)
			{
				rest--;
				mLoseOneByte = false;
			}

			// bufor zapelniony dopisz do pliku
			for(int i = 0; i < mDataBuffer.size(); i += 2)
			{
				short shortData =( mDataBuffer[i] << 8) | (mDataBuffer[i+1]);
				*mpStreamShort << shortData << std::endl;
			}
			for(int i = 0; i < mDataBuffer.size(); i++)
				*mpStreamChar << (int)mDataBuffer[i] << std::endl;
			mpStreamChar->flush();
			mpStreamShort->flush();

			for(int i = 0; i < rest; ++i)
			{
				mDataBuffer[i] = mTempBuffer[i];
			}
			mBufferCursor = rest;
		}
		// jest jeszcze miejsce, przenies dane
		else
		{
			int sizeCopy = mTempBuffer.size();

			// jesli mamy zgubic jeden bajt
			if(mLoseOneByte)
			{
				sizeCopy--;
				mLoseOneByte = false;
			}

			// skopiuj dane z tymczasowego bufora
			for(int i = 0; i < sizeCopy; ++i)
			{
				mDataBuffer[i + mBufferCursor] = mTempBuffer[i];
			}
			mBufferCursor += sizeCopy;
		}
	}
}

void RealTimeReader::loseOneByte()
{
	mLoseOneByte = true;
}

QByteArray& RealTimeReader::buffer()
{
	return mDataBuffer;
}

int RealTimeReader::bufferCursor() const
{
	return mBufferCursor;
}
