#pragma once

#include <QString>

class Exception
{
public:
	Exception(const QString& msg): mMessage(msg) {}
	// Zwraca tresc wyjatku.
	inline const QString& message() const { return mMessage; }
protected:
	QString mMessage; // Tresc wyjatku
};

// pomocne makro do rzucania wyjatkow
#define throw_Exception(ErrorText) \
	throw Exception(QString(__FUNCTION__) + QString(" at line ") \
	+ QString().setNum(__LINE__) + QString(" - ") + ErrorText);
