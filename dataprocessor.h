#pragma once

#include <QByteArray>
#include <fstream>

// forward declarations
class DigitalFilter;

class DataProcessor
{
public:
    DataProcessor();
	~DataProcessor();

	enum DataType {
		Undefined = -1,
		Unsigned8Bits,
		Signed8Bits,
		Unsigned10Bits,
		Signed10Bits,
		Unsigned12Bits,
		Signed12Bits,
		Unsigned16Bits,
		Signed16Bits
	};

	// podany bufor bedzie wypelniany z kazdym wywolaniem calc()
	void setDataType(DataType dataType);
	void setGraphBuffer(float* data, int size);
	void calc(QByteArray& buffer, int bufferCursor, bool useFilter);

	int graphCursor(){ return mGraphCursor; }
	int maxValue() const;
	int minValue() const;
private:
	// wskaznik na bufor dla wykresu
	float* mGraphBuffer;
	// rozmiar bufora
	int mGraphBufferSize;
	// gdzie aktualnie sa zapisywane dane
	float mGraphCursor;
	// rodzaj danych (m.in od nich zalezy jak skalowac wykres)
	DataType mDataType;

	DigitalFilter* mFilter;
	std::ofstream* mpStream;

	// 'interpoluje' dane surowe dla bufora wykresu
	float findValue(int idx, QByteArray& buffer, bool useFilter);
};
