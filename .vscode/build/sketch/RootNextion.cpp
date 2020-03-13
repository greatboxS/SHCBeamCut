// 
// 
// 

#include "RootNextion.h"

void RootNextionClass::init()
{
	NexSerial->begin(Baud);
	while (!NexSerial);
	NexSerial->setTimeout(Timeout);
	PAGE_LOADING_EVENT = new INextionTouchable(*Nex);

	char* prt;

	Serial.println("Initialize Nextion screen number name");
	for (int i = 0; i < TOTAL_NEXTION_NUMBER; i++)
	{
		prt = new char[5]{ '\0' };
		prt[0] = 'n';
		prt[1] = i + 48;
		if (i >= 10)
		{
			prt[1] = i / 10 + 48;
			prt[2] = i % 10 + 48;
		}
		NexNumber[i] = prt;
		Serial.println(NexNumber[i]);
	}

	Serial.println("Initialize Nextion screen text name");
	for (int i = 0; i < TOTAL_NEXTION_TEXT; i++)
	{
		prt = new char[5]{ '\0' };
		prt[0] = 't';
		prt[1] = i + 48;
		if (i >= 10)
		{
			prt[1] = i / 10 + 48;
			prt[2] = i % 10 + 48;
		}
		NexText(i) = prt;
		Serial.println(NexText(i));
	}

	Serial.println("Initialize Nextion screen text name");
	for (int i = 0; i < TOTAL_NEXTION_POTEXT; i++)
	{
		prt = new char[5]{ '\0' };
		prt[0] = 'p';
		prt[1] = 'o';
		prt[2] = i + 48;
		if (i >= 10)
		{
			prt[2] = i / 10 + 48;
			prt[3] = i % 10 + 48;
		}
		NexPo(i) = prt;
		Serial.println(NexPo(i));
	}

	Serial.println("Initialize Nextion screen size name");
	for (int i = 0; i < TOTAL_NEXTION_TEXT; i++)
	{
		prt = new char[5]{ '\0' };
		prt[0] = 's';
		prt[1] = 'q';
		prt[2] = i + 48;
		if (i >= 10)
		{
			prt[2] = i / 10 + 48;
			prt[3] = i % 10 + 48;
		}
		NexSize(i) = prt;
		Serial.println(NexSize(i));
	}
	delete prt;
}


void RootNextionClass::sendCommand(char* commandStr)
{
	NexSerial->print(commandStr);
	NexSerial->write(0xFF);
	NexSerial->write(0xFF);
	NexSerial->write(0xFF);
	log_ln(commandStr);
}

/*!
* \brief Sets the value of a numerical property of this widget.
* \param propertyName Name of the property
* \param value Value
* \return True if successful
*/
void RootNextionClass::setNumberProperty(const char* pageName, char* propertyName, uint32_t value)
{
	size_t commandLen = 8 + strlen(pageName) + strlen(propertyName);
	char commandBuffer[commandLen];
	snprintf(commandBuffer, commandLen, "%s.%s=%ld", pageName, propertyName, value);
	sendCommand(commandBuffer);
}

/*!
 * \brief Gets the value of a numerical property of this widget.
 * \param propertyName Name of the property
 * \return Value (may also return 0 in case of error)
 */
uint32_t RootNextionClass::getNumberProperty(const char* pageName, char* propertyName)
{
	size_t commandLen = 7 + strlen(pageName) + strlen(propertyName);
	char commandBuffer[commandLen];
	snprintf(commandBuffer, commandLen, "get %s.%s", pageName, propertyName);
	sendCommand(commandBuffer);
	uint32_t id;
	if (Nex->receiveNumber(&id))
		return id;
	else
		return 0;
}

/*!
 * \brief Sets the value of a string property of this widget.
 * \param propertyName Name of the property
 * \param value Value
 * \return True if successful
 */
void RootNextionClass::setStringProperty(const char* pageName, char* propertyName, char* value)
{
	size_t commandLen = 7 + strlen(pageName) + strlen(propertyName) + strlen(value);
	char command[commandLen];
	snprintf(command, commandLen, "%s.%s=\"%s\"", pageName, propertyName, value);
	sendCommand(command);
}

/*!
 * \brief Gets the value of a string property of this widget.
 * \param propertyName Name of the property
 * \param value Pointer to char array to store result in
 * \param len Maximum length of value
 * \return Actual length of value
 */
size_t RootNextionClass::getStringProperty(const char* pageName, char* propertyName, char* value,
	size_t len)
{
	size_t commandLen = 6 + strlen(pageName) + strlen(propertyName);
	char command[commandLen];
	snprintf(command, commandLen, "get %s.%s", pageName, propertyName);
	sendCommand(command);
	return Nex->receiveString(value, len);
}

void RootNextionClass::setNumberProperty(const char* propertyName, uint32_t value)
{
	size_t commandLen = 8 + strlen(propertyName);
	char commandBuffer[commandLen];
	snprintf(commandBuffer, commandLen, "%s=%ld", propertyName, value);
	sendCommand(commandBuffer);
}

uint32_t RootNextionClass::getNumberProperty(const char* propertyName)
{
	size_t commandLen = 7 + strlen(propertyName);
	char commandBuffer[commandLen];
	snprintf(commandBuffer, commandLen, "get %s", propertyName);
	sendCommand(commandBuffer);
	uint32_t id;
	if (Nex->receiveNumber(&id))
		return id;
	else
		return 0;
}

void RootNextionClass::setStringProperty(const char* propertyName, char* value)
{
	size_t commandLen = 7 + strlen(propertyName) + strlen(value);
	char command[commandLen];
	snprintf(command, commandLen, "%s=\"%s\"", propertyName, value);
	sendCommand(command);
}

size_t RootNextionClass::getStringProperty(const char* propertyName, char* value, size_t len)
{
	size_t commandLen = 6 + strlen(propertyName);
	char command[commandLen];
	snprintf(command, commandLen, "get %s", propertyName);
	sendCommand(command);
	return Nex->receiveString(value, len);
}

void RootNextionClass::SetPage_stringValue(BKanbanPage_t page, const char* propertyName, char* str)
{
	char temp[32]{ 0 };
	getPropertyName(propertyName, temp, NEX_TEXT);
	setStringProperty(PageName[(uint8_t)page], temp, str);
}

void RootNextionClass::SetPage_numberValue(BKanbanPage_t page, const char* propertyName, uint32_t value)
{
	char temp[32]{ 0 };
	getPropertyName(propertyName, temp, NEX_NUMBER);
	setNumberProperty(PageName[(uint8_t)page], temp, value);
}

uint32_t RootNextionClass::GetPage_numberValue(BKanbanPage_t page, const char* propertyName)
{
	char temp[32]{ 0 };
	getPropertyName(propertyName, temp, NEX_NUMBER);
	return getNumberProperty(PageName[(uint8_t)page], temp);
}

void RootNextionClass::GetPage_stringValue(BKanbanPage_t page, const char* propertyName, char* outputStr, size_t len)
{
	char temp[32]{ 0 };;
	getPropertyName(propertyName, temp, NEX_TEXT);
	getStringProperty(PageName[(uint8_t)page], temp, outputStr, len);
}

void RootNextionClass::SetPage_propertyBackgroundColor(BKanbanPage_t page, const char* propertyName, uint32_t color)
{
	char temp[32]{ 0 };
	getPropertyName(propertyName, temp, NEX_BACKGROUND_COLOR);
	setNumberProperty(PageName[(uint8_t)page], temp, color);
}

void RootNextionClass::SetPage_propertyForceColor(BKanbanPage_t page, const char* propertyName, uint32_t color)
{
	char temp[32]{ 0 };
	getPropertyName(propertyName, temp, NEX_FORCE_COLOR);
	setNumberProperty(PageName[(uint8_t)page], temp, color);
}

void RootNextionClass::setPage_stringAsNumberProperty(uint8_t page, const char* propertyName, uint32_t value)
{
	char temp[32]{ 0 };
	getPropertyName(propertyName, temp, NEX_TEXT);
	char Number[6];
	snprintf(Number, sizeof(Number), "%d", value);
	setStringProperty(PageName[(uint8_t)page], temp, Number);
}

void RootNextionClass::setPage_stringAsNumberProperty(uint8_t page, char* propertyName, uint32_t value)
{
	char temp[32]{ 0 };
	getPropertyName(propertyName, temp, NEX_TEXT);
	char Number[6];
	snprintf(Number, sizeof(Number), "%d", value);
	setStringProperty(PageName[(uint8_t)page], temp, Number);
}

void RootNextionClass::SetPage_stringValue(BKanbanPage_t page, const char* propertyName, const char* str)
{
	char temp[32]{ 0 };
	uint8_t strLen = strlen(str);
	char _str[strLen + 10]{ 0 };
	memccpy(_str, str, 0, strLen);
	getPropertyName(propertyName, temp, NEX_TEXT);
	setStringProperty(PageName[(uint8_t)page], temp, _str);
}
