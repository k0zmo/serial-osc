#pragma once

#include <windows.h>
#include <fstream>

#include <QtCore>
#include <QByteArray>

class RealTimeReader
{
public:
    RealTimeReader();
	~RealTimeReader();

	void open(const QString& port, int baudRate);
	void close();

	// musimy wiedziec jak duzy bufor zaalokowac
	void setReadingParams(int bytesPerSec, int maxSeconds);
	void read();

	// przeskakuje jeden bajt w nastepnym odczycie
	void loseOneByte();

	QByteArray& buffer();
	int bufferCursor() const;
	bool opened() const { return (mFile != INVALID_HANDLE_VALUE); }

private:
	HANDLE mFile;

	int mBytesPerSec;
	int mMaxSeconds;
	int mMaxBufferSize;
	int mBufferCursor;

	QByteArray mTempBuffer;
	QByteArray mDataBuffer;

	bool mLoseOneByte;

	std::ofstream* mpStreamChar;
	std::ofstream* mpStreamShort;
};
