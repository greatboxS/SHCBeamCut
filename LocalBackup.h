#ifndef __LOCAL_BACKUP_H
#define __LOCAL_BACKUP_H

#define DATE_TIME_START_ADDRESS 0

#define PO_ID_ADDRESS 10

#define SEQ_ID_ADDRESS 14

#define COMPONENT_ID_ADDRESS 18

#define MACHINE_ID_ADDRESS 22

#define MACHINE_SYS_CODE_ADDRESS 26

#define MACHINE_NAME_ADDRESS 36

#define INTERFACE_ID_ADDRESS 64

#define SERVER_IP_ADDRESS 100

#define SERVER_PORT_ADDRESS 132

#include "EEPROM.h"

namespace lcal
{

void SaveTime(uint8_t *_datetimeArr, bool isReading);

template <typename T>
void SaveCuttingInfo(T _poId, T _seqId, T _comId, T _interface);

template <typename T>
void ReadCuttingInfo(T &_poId, T &_seqId, T &_comId, T &_interface);

template <typename T>
void write(T _value, int _address);

template <typename T>
T read(int _address);

void SaveMachineInfo(int _machineId, char *_sysCode, char *_machineName, uint8_t _len);

void ReadMachineInfo(int &_machineId, char *_sysCode, char *_machineName, uint8_t _len);

//------------------------------------------------------------------------------
void SaveTime(uint8_t *_datetimeArr, bool isReading)
{
	EEPROM.begin(512);

	for (uint8_t i = 0; i < 6; i++)
	{
		if (!isReading)
			EEPROM.write(DATE_TIME_START_ADDRESS + i, _datetimeArr[i]);
		else
			_datetimeArr[i] = EEPROM.read(DATE_TIME_START_ADDRESS + i);
	}

	EEPROM.end();
}

template <typename T>
void SaveCuttingInfo(T _poId, T _seqId, T _comId, T _interfaceId)
{
	EEPROM.begin(512);
	write<T>(_poId, PO_ID_ADDRESS);
	write<T>(_seqId, SEQ_ID_ADDRESS);
	write<T>(_comId, COMPONENT_ID_ADDRESS);
	write<T>(_interfaceId, INTERFACE_ID_ADDRESS);
	EEPROM.end();
}

template <typename T>
void ReadCuttingInfo(T &_poId, T &_seqId, T &_comId, T &_interface)
{
	EEPROM.begin(512);
	Serial.println(_poId);
	Serial.println(_seqId);
	Serial.println(_comId);

	_poId = read<T>(PO_ID_ADDRESS);
	_seqId = read<T>(SEQ_ID_ADDRESS);
	_comId = read<T>(COMPONENT_ID_ADDRESS);
	_interface = read<T>(INTERFACE_ID_ADDRESS);
	EEPROM.end();
}

template <typename T>
void write(T _value, int _address)
{
	uint8_t size = sizeof(T);
	Serial.println(size);

	for (uint8_t i = 0; i < size; i++)
	{
		uint8_t value = _value >> ((size - 1 - i) * 8);
		EEPROM.write(_address + i, value);
		Serial.println(value, HEX);
	}
	Serial.println(_value);
}

template <typename T>
T read(int _address)
{
	uint8_t size = sizeof(T);
	Serial.println(size);
	T result;

	for (uint8_t i = 0; i < size; i++)
	{
		uint8_t sLeft = (size - 1 - i) * 8;
		uint8_t v = EEPROM.read(_address + i);
		result |= ((T)v << sLeft);
		Serial.println(result, HEX);
	}
	Serial.println(result);
	return result;
}

void SaveMachineInfo(int _machineId, char *_sysCode, char *_machineName, uint8_t _len)
{
	EEPROM.begin(512);
	write(_machineId, MACHINE_ID_ADDRESS);

	for (size_t i = 0; i < 10; i++)
	{
		EEPROM.write(MACHINE_SYS_CODE_ADDRESS + i, *_sysCode);
		_sysCode++;
		Serial.print(*_sysCode);
	}

	for (size_t i = 0; i < _len; i++)
	{
		EEPROM.write(MACHINE_NAME_ADDRESS + i, *_machineName);
		_machineName++;
		Serial.print(*_machineName);
	}

	EEPROM.end();
}

void ReadMachineInfo(int &_machineId, char *_sysCode, char *_machineName, uint8_t _len)
{
	EEPROM.begin(512);
	_machineId = read<int>(MACHINE_ID_ADDRESS);

	for (size_t i = 0; i < 10; i++)
	{
		*_sysCode = EEPROM.read(i + MACHINE_SYS_CODE_ADDRESS);
		_sysCode++;
		Serial.print(*_sysCode);
	}

	for (uint8_t i = 0; i < _len; i++)
	{
		*_machineName = EEPROM.read(i + MACHINE_NAME_ADDRESS);
		_machineName++;
		Serial.print(*_machineName);
	}

	EEPROM.end();
}

template <typename T>
void WriteServerPort(T port)
{
	EEPROM.begin(512);
	write(port, SERVER_PORT_ADDRESS);
	EEPROM.end();
	Serial.println(port);
}

template <typename T>
void ReadServerPort(T &port)
{
	EEPROM.begin(512);
	port = read<T>(SERVER_PORT_ADDRESS);
	EEPROM.end();
}

void ReadServerIp(char *serverIp, size_t len)
{
	EEPROM.begin(512);
	for (size_t i = SERVER_IP_ADDRESS; i < SERVER_IP_ADDRESS + len; i++)
	{
		*serverIp = EEPROM.read(i);
		++serverIp;
	}
	EEPROM.end();
}

void WriteServerIp(char *serverIp, size_t len)
{
	EEPROM.begin(512);
	for (size_t i = SERVER_IP_ADDRESS; i < SERVER_IP_ADDRESS + len; i++)
	{
		EEPROM.write(i, *serverIp);
		++serverIp;
	}
	EEPROM.end();
}

template <typename T>
void SaveServerInformation(char *serverIp, uint8_t len, T &port)
{
	WriteServerIp(serverIp, len);
	WriteServerPort(port);
}

template <typename T>
void ReadServerInformation(char *serverIp, uint8_t len, T &port)
{
	ReadServerIp(serverIp, len);
	ReadServerPort(port);
	Serial.println(port);
}
} // namespace lcal

#endif