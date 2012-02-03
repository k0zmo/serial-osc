#pragma once

#include <QtCore>
#include <QByteArray>

class DigitalFilter
{
public:
    DigitalFilter();
	~DigitalFilter();

	void doFiltering(QByteArray& samples, int count);
	void doFiltering(float sample, float& sampleOut);
	QVarLengthArray<float>& getFilteredSamples() { return samplesOut; }

private:
	// na razie jest to przypisanie twarde
	float a[14][3], b[14][3];
	float gain_in[14], gain_out;
	float x, y;
	float w[14][3];
	int num_sections;

	QVarLengthArray<float> samplesOut;
};
