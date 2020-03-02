#include "MFRC_Lib.h"

MFRC_Lib::MFRC_Lib(byte _csPin, byte _rstPin)
{
	mfrc = MFRC522(_csPin, _rstPin);
}

void MFRC_Lib::init()
{
	mfrc.PCD_Init();
}

bool MFRC_Lib::tag_detected()
{
	if (mfrc.PICC_IsNewCardPresent())
	{
		if (mfrc.PICC_ReadCardSerial())
		{
			mfrc.PICC_HaltA();
			mfrc.PCD_StopCrypto1();
			return true;
		}
	}
	return false;
}

String MFRC_Lib::read_tagNumber()
{
	String uid = "";

	for (int8_t i = 0; i < 4; i++)
	{
		(mfrc.uid.uidByte[3 - i] < 0x10) ? uid += '0' : "";

		uid += String(mfrc.uid.uidByte[3 - i], HEX);
	}
	uid.toUpperCase();
	return uid;
}

void MFRC_Lib::clear_tag_number()
{
	for (int i = 0; i < sizeof(mfrc.uid.uidByte); i++)
	{
		mfrc.uid.uidByte[i] = 0;
	}
}
