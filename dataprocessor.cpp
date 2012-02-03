#include "pch.h"
#include "dataprocessor.h"
#include "digitalfilter.h"

DataProcessor::DataProcessor()
	: mGraphBuffer(0), mGraphBufferSize(0), mGraphCursor(0),
	mDataType(Undefined)
{

	mFilter = new DigitalFilter;
	
	mpStream = new std::ofstream;
	mpStream->open("data_float.txt", std::ios::binary | std::ios::out);
}

DataProcessor::~DataProcessor()
{
	mpStream->close();
	delete mpStream;
	delete mFilter;
}

void DataProcessor::setDataType(DataType dataType)
{
	mDataType = dataType;
}

void DataProcessor::setGraphBuffer(float* data, int size)
{
	mGraphBuffer = data;
	mGraphBufferSize = size;
}

void DataProcessor::calc(QByteArray& buffer, int bufferCursor, bool useFilter)
{
	static int prevT = 0;
	static int nowT = 0;
	static int nextT;
	
	// policz pozycje dla bufora wykresu
	nextT = static_cast<int>(
		(static_cast<float>(bufferCursor)/static_cast<float>(buffer.size())) * mGraphBufferSize
	);
	mGraphCursor = nowT;

	if((nowT == 0) && (prevT == 0))
	{
		// kolejny cykl
		prevT = nowT;
		nowT = nextT;
		return;
	}

	if(nowT > prevT){
		
		for(int i = prevT; i < nowT; ++i)
			mGraphBuffer[i] = findValue(i, buffer, useFilter);
	}
	else
	{
		// mamy zapetlenie, dokoncz petle
		for(int i = prevT; i < mGraphBufferSize; ++i)
			mGraphBuffer[i] = findValue(i, buffer, useFilter);

		// zapis danych na dysk
		for(int i = 0; i < mGraphBufferSize; ++i)
			*mpStream << (float) mGraphBuffer[i] << std::endl;

		// zacznij od nowa
		for(int i = 0; i < nowT; ++i)
			mGraphBuffer[i] = findValue(i, buffer, useFilter);
	}

	// kolejny cykl
	prevT = nowT;
	nowT = nextT;
}

float DataProcessor::findValue(int idx, QByteArray& buffer, bool useFilter)
{
	// [t,k] = [0,1]
	// dla przeskalowania indeksow z bufora danych do bufora wykresu
	float t = static_cast<float>(idx) / mGraphBufferSize;
	float k = static_cast<float>(idx + 1) / mGraphBufferSize;

	// przedzial [startT, endT] to indeksy wartosci z bufora danych
	// ktore odpowiadaja dla wartosci o indeksie idx z bufora wykresu
	int startT = static_cast<int>(t * static_cast<float>(buffer.size()));
	int endT = static_cast<int>(k * static_cast<float>(buffer.size()));

	// 'interpolowana' wartosc
	float retVal = 0.0f;
	int sumSize = 0;

	// dla k=1 wyszlibysmy poza zakres
	if(endT >= buffer.size())
		endT = buffer.size() - 1;

	// TODO:
	// (startT == endT) dla buffer.size() <= mGraphBufferSize
	// taki przypadek nalezy rowniez obsluzyc

	unsigned char data1;
	unsigned char data2;
	unsigned short shortData;

	// startT dla 2bitowych danych powininen byc parzysty
	// endT dla 2bitowych danych powinien byc parzysty
	if((startT % 2) && (mDataType >= Unsigned10Bits))
		startT--;
	if((endT % 2) && (mDataType >= Unsigned10Bits))
		endT--;

	float sample;
	for(int i = startT; i < endT; ++i)
	{
		switch(mDataType)
		{
		case Unsigned8Bits:
			sample = static_cast<float>(static_cast<unsigned char>(buffer[i]));
			break;
		case Signed8Bits:
			sample = static_cast<float>(static_cast<signed char>(buffer[i]));
			break;
		case Unsigned10Bits:
		case Unsigned12Bits:
		case Unsigned16Bits:
			// TODO:
			// nalezaloby sprawdzic czy mozna zrobic i++ (dla malych buforow)
			data1 = static_cast<unsigned char>(buffer[i++]);
			data2 = static_cast<unsigned char>(buffer[i]);
			// polacz dwa bity
			shortData = (data1<<8) | (data2);
			sample = static_cast<float>(shortData);
			//printf("%d\t%d\t%d\n", data1, data2, ushortData);
			break;
		case Signed10Bits:
		case Signed12Bits:
		case Signed16Bits:
			// TODO:
			// nalezaloby sprawdzic czy mozna zrobic i++ (dla malych buforow)
			data1 = static_cast<unsigned char>(buffer[i++]);
			data2 = static_cast<unsigned char>(buffer[i]);
			// polacz dwa bity
			shortData = (data1<<8) | (data2);
			sample = static_cast<float>(static_cast<signed short>(shortData));
			//printf("%d\t%d\t%d\n", data1, data2, ushortData);
			break;
		default:
			break;
		}

		if(useFilter)
		{
			float flt;
			mFilter->doFiltering(sample, flt);
			retVal += flt;
		}
		else
		{
			 retVal += sample;
		}

		++sumSize;
	}
	retVal /= static_cast<float>(sumSize);
	//printf("%f\n", retVal);
	return retVal;
}

int DataProcessor::minValue() const
{
	switch(mDataType)
	{
	case Unsigned8Bits:
	case Unsigned10Bits:
	case Unsigned12Bits:
	case Unsigned16Bits:
		return 0;
		break;
	case Signed8Bits:
		return -128;
		break;
	case Signed10Bits:
		return -512;
		break;
	case Signed12Bits:
		return -2048;
		break;
	case Signed16Bits:
		return -32768;
		break;
	default:
		return 0;
		break;
	}
}

int DataProcessor::maxValue() const
{
	switch(mDataType)
	{
	case Unsigned8Bits:
		return 255;
		break;
	case Signed8Bits:
		return 127;
		break;
	case Unsigned10Bits:
		return 1023;
		break;
	case Signed10Bits:
		return 511;
		break;
	case Unsigned12Bits:
		return 4095;
		break;
	case Signed12Bits:
		return 2047;
		break;
	case Unsigned16Bits:
		return 65535;
		break;
	case Signed16Bits:
		return 32767;
		break;
	default:
		return 0;
		break;
	}
}

