#ifndef _MFRC_LIB_H
#define _MFRC_LIB_H

#include <string.h>
#include <SPI.h>
#include "MFRC522.h"

class MFRC_Lib
{
public:
	MFRC_Lib(MFRC522& _mfrc):mfrc(_mfrc){}

	MFRC_Lib(byte _csPin, byte _rstPin);

	void init();

	bool tag_detected();

	String read_tagNumber();

	void clear_tag_number();

private:
	MFRC522 mfrc;
};

#endif // !_MFRC_LIB_H








