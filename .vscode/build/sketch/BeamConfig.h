#ifndef _BeamConfig_h
#define _BeamConfig_h

#include "Arduino.h"
#include "RootNextion.h"
#include "EthernetConfig.h"
#include "LocalBackup.h"
#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
#include <esp_task_wdt.h>
#include <esp_int_wdt.h>
#include <EEPROM.h>
//

// #ifdef STM32
// // Hardware serial
// #define NEXTION_RX_PIN PA3
// #define NEXTION_TX_PIN PA2

// #define EXTERNAL_INTERRUPT_PIN PB0
// #define ALARM_PIN PB1
// #define ETHERNET_RST_PIN PB12
// #define ETHERNET_SPI_CS_PIN PA4
// #define MFRC_SPI_CS_PIN PB13
// #define MFRC_SPI_RST_PIN PB14
// #else
// // cs GPIO34
// // rst GPIO35
// #define EXTERNAL_INTERRUPT_PIN GPIO_NUM_13
// #define ALARM_PIN GPIO_NUM_5
// #define ETHERNET_RST_PIN GPIO_NUM_26	//26
// #define ETHERNET_SPI_CS_PIN GPIO_NUM_25 //25
// #define MFRC_SPI_CS_PIN GPIO_NUM_32
// #define MFRC_SPI_RST_PIN GPIO_NUM_33
// #define TOUCH_BUTTON GPIO_NUM_15
// #endif
// esp SPI pinout
// miso gpio19 D19
// sck gpio18 D18
// mosi gpio23 D23
// cs gpio22 D22 // optional

// UART1
// RX GPIO3
// TX GPIO1

// UART2
// RX GPIO16
// TX GPIO17

// TOUCH
// T0 D4
// T2 D2
// T3 D15

#define EXTERNAL_INTERRUPT_PIN GPIO_NUM_5
#define ALARM_PIN GPIO_NUM_13
#define ETHERNET_RST_PIN GPIO_NUM_26	//26
#define ETHERNET_SPI_CS_PIN GPIO_NUM_25 //25
#define RST_PIN GPIO_NUM_33				// Configurable, see typical pin layout above
#define SS_PIN GPIO_NUM_32				// Configurable, see typical pin layout above
#define TOUCH_BUTTON GPIO_NUM_15

#define SAVE_INTERFACE_ID_ADDR 140
#define SAVE_CURRENT_CUT_TIME 145
#define SAVE_TOTAL_CUT_TIME 150
#define SAVE_USER_ID_ADDR 160
#define SAVE_USER_CODE_ADDR 164
#define SAVE_USER_RFID_ADDR 168
#define SAVE_USER_DEP_ADDR 188
#define SAVE_USER_NAME_ADDR 200
#define SAVE_MAC_ADDR 300

RootNextionClass RootNextion = RootNextionClass(2, 9600, 200);
HttpHeader_t httpHeader;
BKanban_t BKanban;
SysFlag_t Flag;
RequestQueue_t RequestQueue;
Schedule_t SelectedSchedule;
MFRC522 mfrc = MFRC522(SS_PIN, RST_PIN);
RootEthernetClass RootEthernet;

uint32_t oldTick = 0;
uint8_t Timeout = 0;
bool SubmitCutTimeNow = false;
bool EthernetStatus = false;
bool AddNewUser = false;
int last_cut_timeout_tick = 0;
bool get_last_cut_timeout = false;

int EthernetInitialTimeout = 0;
bool EthernetRenew = true;
bool get_lastcut_ok = false;

volatile int TouchTimes = 0;
volatile bool settingMode = false;
bool responsed_failed = false;

void ExternalInterrupt();
void DataReceiveCallBack(EthernetClient &stream);
void EEPROM_SaveTime(bool isRead = false);
void EEPROM_SaveMachineInfo(bool isRead = false);
void EEPROM_SaveCuttingInfo(bool isRead = false);
void EEPROM_SaveServerInfo(bool isRead = false);
void EEPROM_SaveMac(bool isRead, byte *buf);

void ReadCuttingParameter()
{
	touch_pad_intr_disable();
	EEPROM.begin(512);
	BKanban.BInterface.BinterfaceId = lcal::read<int>(SAVE_INTERFACE_ID_ADDR);
	BKanban.Cutting.CurrentCutTimes = lcal::read<int>(SAVE_CURRENT_CUT_TIME);
	BKanban.Cutting.TotalCutTimes = lcal::read<int>(SAVE_TOTAL_CUT_TIME);
	EEPROM.end();
	touch_pad_intr_enable();
}

void SaveCuttingInterfaceId()
{
	touch_pad_intr_disable();
	EEPROM.begin(512);
	lcal::write<int>(BKanban.BInterface.BinterfaceId, SAVE_INTERFACE_ID_ADDR);
	EEPROM.end();
	touch_pad_intr_enable();
}

void SaveTotalCutTimes()
{
	touch_pad_intr_disable();
	EEPROM.begin(512);
	lcal::write<int>(BKanban.Cutting.TotalCutTimes, SAVE_TOTAL_CUT_TIME);
	EEPROM.end();
	touch_pad_intr_enable();
}

void SaveCuttingParameter()
{
	touch_pad_intr_disable();
	EEPROM.begin(512);
	lcal::write<int>(BKanban.BInterface.BinterfaceId, SAVE_INTERFACE_ID_ADDR);
	lcal::write<int>(BKanban.Cutting.CurrentCutTimes, SAVE_CURRENT_CUT_TIME);
	lcal::write<int>(BKanban.Cutting.TotalCutTimes, SAVE_TOTAL_CUT_TIME);
	EEPROM.end();
	touch_pad_intr_enable();
}
void SaveUserInfo();
void ReadUserInfo();
void TouchButton_Callback();

bool tag_detected();
String read_tagNumber();
void clear_tag_number();

void IO_Init();
void RootNextion_Init();
void Ethernet_Init();
void InitializeBKanban();
void Output_Alarm(uint8_t count = 1);
void SysStartup();

void PAGE_LOADING_EVENT_CALLBACK(uint8_t _pageId, uint8_t _componentId, uint8_t _eventType);

#include "EthernetRequest.h"
#include "RootNextionCallback.h"

/////////////////////////////////////////////////////////////////////////////////
void SysStartup()
{
	RootNextion_Init();
	IO_Init();
	SPI.begin();
	mfrc.PCD_Init();				// Init MFRC522
	delay(4);						// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
	InitializeBKanban();
	Ethernet_Init();
	Serial.println("Initialize finish");
}
/////////////////////////////////////////////////////////////////////////////////
void ExternalInterrupt()
{
	f_log();
	if (millis() - oldTick > 2000)
	{
		oldTick = millis();
		if (digitalRead(EXTERNAL_INTERRUPT_PIN) == HIGH)
			return;

		log_ln("Detected new cut time");
		BKanban.Cutting.TotalCutTimes++;
		BKanban.Cutting.SubmitCutTime++;
	}
}

void SaveUserInfo()
{
	f_log();
	touch_pad_intr_disable();
	EEPROM.begin(512);
	lcal::write<int>(BKanban.Cutting.Worker.id, SAVE_USER_ID_ADDR);
	lcal::write<int>(BKanban.Cutting.Worker.UserCode, SAVE_USER_CODE_ADDR);
	for (size_t i = 0; i < sizeof(BKanban.Cutting.Worker.UserRFID); i++)
	{
		lcal::write(BKanban.Cutting.Worker.UserRFID[i], i + SAVE_USER_RFID_ADDR);
	}

	for (size_t i = 0; i < BKanban.Cutting.Worker.Department.length(); i++)
	{
		lcal::write(BKanban.Cutting.Worker.Department[i], i + SAVE_USER_DEP_ADDR);
	}

	for (size_t i = 0; i < sizeof(BKanban.Cutting.Worker.UserName); i++)
	{
		lcal::write(BKanban.Cutting.Worker.UserName[i], i + SAVE_USER_NAME_ADDR);
	}
	EEPROM.end();
	touch_pad_intr_enable();
}

void ReadUserInfo()
{
	f_log();
	touch_pad_intr_disable();
	EEPROM.begin(512);
	BKanban.Cutting.Worker.id = lcal::read<int>(SAVE_USER_ID_ADDR);
	BKanban.Cutting.Worker.UserCode = lcal::read<int>(SAVE_USER_CODE_ADDR);

	for (size_t i = 0; i < sizeof(BKanban.Cutting.Worker.UserRFID); i++)
	{
		BKanban.Cutting.Worker.UserRFID[i] = lcal::read<char>(i + SAVE_USER_RFID_ADDR);
	}

	char temp[10]{0};
	for (size_t i = 0; i < 10; i++)
	{
		temp[i] = lcal::read<char>(i + SAVE_USER_DEP_ADDR);
	}

	BKanban.Cutting.Worker.Department = String(temp);

	for (size_t i = 0; i < sizeof(BKanban.Cutting.Worker.UserName); i++)
	{
		BKanban.Cutting.Worker.UserName[i] = lcal::read<char>(i + SAVE_USER_NAME_ADDR);
	}
	EEPROM.end();
	log_ln(BKanban.Cutting.Worker.id);
	log_ln(BKanban.Cutting.Worker.UserCode);
	log_ln(BKanban.Cutting.Worker.Department);
	log_ln(BKanban.Cutting.Worker.UserName);
	touch_pad_intr_enable();
}

void TouchButton_Callback()
{
	f_log();
	TouchTimes++;
	if (TouchTimes >= 10)
	{
		settingMode = true;
		log_ln("Go to setting mode now");
	}
}

void IO_Init()
{
	Serial.begin(115200);
	while (!Serial)
		;

	Serial.setTimeout(50);
	Serial.println("Initialize Serial");
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	pinMode(ALARM_PIN, OUTPUT);
	digitalWrite(ALARM_PIN, HIGH);
	pinMode(ETHERNET_RST_PIN, OUTPUT);
	digitalWrite(ETHERNET_RST_PIN, HIGH);
	pinMode(ETHERNET_SPI_CS_PIN, OUTPUT);
	digitalWrite(ETHERNET_SPI_CS_PIN, HIGH);

	//touchAttachInterrupt(TOUCH_BUTTON, TouchButton_Callback, 40);
	pinMode(EXTERNAL_INTERRUPT_PIN, INPUT);
	attachInterrupt(EXTERNAL_INTERRUPT_PIN, ExternalInterrupt, FALLING);
	delay(1000);

	int timeout = 60000 * 10;
	if (settingMode)
	{
		touch_pad_intr_disable();
		RootNextion.showMessage("Setting your device");
		delay(500);
		ButSettingMachineClickCallback();
	}

	while (settingMode)
	{
		RootNextion.Listening();
		if (timeout < 0)
		{
			esp_restart();
		}
		timeout -= 10;
		log_ln("Setting mode");
		delay(10);
	}
	touch_pad_intr_enable();
}

void RootNextion_Init()
{
	RootNextion.init();
	RootNextion.PAGE_LOADING_EVENT->root_attachCallback(PAGE_LOADING_EVENT_CALLBACK);
}

void Ethernet_Init()
{
	RootNextion.Waiting(60000, "Initializing Ethernet module...");
	RootEthernet.init(ETHERNET_SPI_CS_PIN, ETHERNET_RST_PIN, DataReceiveCallBack);

	while (RootEthernet.begin() == 0)
	{
		delay(100);
		RootNextion.Listening();
	}

	RootNextion.Waiting(20000, "Connecting to server...");

	RootEthernet.begin_connection(20000);
	RootNextion.GotoPage(HIS_PAGE);

	Ethernet_GetTime();
	RootNextion.Waiting(10000, "Connected, Checking last cut...");
	Ethernet_GetLastCut();
}

void InitializeBKanban()
{
	touch_pad_intr_disable();
	f_log();
	lcal::ReadServerInformation(RootEthernet.server, sizeof(RootEthernet.server), RootEthernet.port);
	RootEthernet.SetupHost();

	lcal::ReadMachineInfo(BKanban.Machine.id, BKanban.Machine.MachineCode, BKanban.Machine.MachineName, sizeof(BKanban.Machine.MachineName));
	RootNextion.SetPage_stringValue(HIS_PAGE, RootNextion.HisPageHandle.DEVICE_NAME, BKanban.Machine.MachineName);

	ReadCuttingParameter();

	ReadUserInfo();

	byte tempMac[6]{0};
	bool readMac = true;
	log_ln("Read Mac");
	EEPROM_SaveMac(true, tempMac);
	for (int i = 0; i < sizeof(tempMac); i++)
	{
		if (tempMac[i] == 0 || tempMac[i] == 255 || tempMac[i] == -1)
			readMac = false;
	}

	if (readMac)
		memccpy(RootEthernet.mac, tempMac, 0, sizeof(tempMac));
	touch_pad_intr_enable();
}

void Output_Alarm(uint8_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		digitalWrite(ALARM_PIN, LOW);
		delay(50);
		digitalWrite(ALARM_PIN, HIGH);
		delay(50);
	}
}

bool tag_detected()
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

String read_tagNumber()
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

void clear_tag_number()
{
	for (int i = 0; i < sizeof(mfrc.uid.uidByte); i++)
	{
		mfrc.uid.uidByte[i] = 0;
	}
}

void DataReceiveCallBack(EthernetClient &stream)
{
	int receivedBytes = stream.available();
	memset(httpHeader.buf, 0, sizeof(httpHeader.buf));
	stream.readBytesUntil('\r\n', httpHeader.buf, receivedBytes);

	Flag.IsRequest = false;
	responsed_failed = false;
	if (strstr(httpHeader.buf, "200 OK") != nullptr)
	{
		log_ln(httpHeader.buf);
		responsed_failed = false;
	}

	Serial.printf("%s\r\n", httpHeader.buf);

	if (strstr(httpHeader.buf, "Access denied") != nullptr)
	{
		responsed_failed = true;
		RootNextion.showMessage("Access denied", 2000);
	}

	if (strstr(httpHeader.buf, "400 Bad Request") != nullptr)
	{
		responsed_failed = true;
		RootNextion.showMessage("400 Bad Request", 2000);
	}

	if (strstr(httpHeader.buf, "401 Unauthorized") != nullptr)
	{
		responsed_failed = true;
		RootNextion.showMessage("401 Unauthorized", 2000);
	}

	if (strstr(httpHeader.buf, "408 Request Timeout") != nullptr)
	{
		responsed_failed = true;
		RootNextion.showMessage("408 Request Timeout", 2000);
	}

	if (strstr(httpHeader.buf, "501 Not Implemented") != nullptr)
	{
		responsed_failed = true;
		RootNextion.showMessage("501 Not Implemented", 2000);
	}

	if (strstr(httpHeader.buf, "502 Bad Gateway") != nullptr)
	{
		responsed_failed = true;
		RootNextion.showMessage("502 Bad Gateway", 2000);
	}

	if (strstr(httpHeader.buf, "503 Service Unavailable") != nullptr)
	{
		responsed_failed = true;
		RootNextion.showMessage("503 Service Unavailable", 2000);
	}

	if (responsed_failed)
	{
		RootNextion.GotoPage(BKanban.CurrentWindowId);
		return;
	}

	if (strstr(httpHeader.buf, "{") != nullptr)
	{
		// Get the responsed data from server
		DynamicJsonDocument JsonDoc = DynamicJsonDocument(1024 * 3);
		DeserializationError JsonErr = deserializeJson(JsonDoc, httpHeader.buf);

		log_ln(JsonErr.c_str());

		if (!JsonErr)
		{
			JsonObject Exception = JsonDoc.getMember("Exception");
			String eop = Exception.getMember("eop").as<const char *>();
			bool Error = Exception.getMember("Error").as<bool>();
			String Message = Exception.getMember("Message").as<const char *>();

			BKanban.EthernetRespType = RESP_ERROR;

			if (Error)
			{
				Flag.RespError = true;
				if (Message.c_str() != NULL)
				{
					Message.toLowerCase();
					if (Message.indexOf("can not find binterface") > -1)
					{
						get_lastcut_ok = true;
						get_last_cut_timeout = false;
					}
					RootNextion.showMessage(Message.c_str());
				}
				return;
			}

			if (eop.c_str() != NULL)
				log_ln(eop.c_str());

			if (eop == "GET_TIME")
			{
				BKanban.EthernetRespType = RESP_GET_TIME;
				BKanban.Json_UpdateTime(JsonDoc);
				Nextion_UpdateTime();
				Ethernet_GetLastCut();
			}

			if (eop == "BEAM_GET_SCHEDULE")
			{
				BKanban.EthernetRespType = RESP_GET_SCHEDULE;
				BKanban.Cutting.Json_UpdateScheduleList(JsonDoc);
				RootNextion.GotoPage(SEARCH_PAGE);
			}

			if (eop == "BEAM_GET_PO_INFO")
			{
				BKanban.EthernetRespType = RESP_GET_SCHEDULE_INFO;
				BKanban.Cutting.Json_UpdateScheduleInfo(JsonDoc);
				delay(100);
				if (BKanban.Cutting.Continue)
				{
					Ethernet_GetSize(BKanban.BInterface);
				}
				else
					RootNextion.GotoPage(INFO_PAGE);
			}

			if (eop == "GET_USER_INFO")
			{
				BKanban.EthernetRespType = RESP_GET_USER;
				BKanban.Cutting.Worker.UpdateUser(JsonDoc);
				Nextion_UpdateUserPage();
				SaveUserInfo();
				if (AddNewUser)
				{
					RootNextion.showMessage("Add new user success", 2000);
					AddNewUser=false;
				}
				else
					RootNextion.GotoPage(LOGIN_PAGE);
			}

			if (eop == "BEAM_GET_SIZE")
			{
				BKanban.EthernetRespType = RESP_GET_SIZE;
				BKanban.Cutting.Json_UpdateSizeList(JsonDoc);

				if (BKanban.Cutting.Continue)
				{
					Ethernet_StartCutting(BKanban.BInterface);
				}
				RootNextion.GotoPage(CUTTING_PAGE);
			}

			if (eop == "GET_DEVICE_INFO")
			{
				BKanban.EthernetRespType = RESP_GET_MACHINE;
				BKanban.Machine.Json_UpdateMachineInfo(JsonDoc);
				lcal::SaveMachineInfo(BKanban.Machine.id, BKanban.Machine.MachineCode, BKanban.Machine.MachineName, sizeof(BKanban.Machine.MachineName));
				RootNextion.SetPage_stringValue(HIS_PAGE, RootNextion.HisPageHandle.DEVICE_NAME, BKanban.Machine.MachineName);
				RootNextion.GotoPage(MACHINE_PAGE);
			}

			if (eop == "BEAM_GET_COMPONENT")
			{
				BKanban.EthernetRespType = RESP_GET_COMPONENT;
				BKanban.Cutting.Json_UpdateComponentList(JsonDoc);

				if (BKanban.Cutting.Continue)
				{
					BKanban.SelectedComponent = BKanban.Cutting.AutoSelectCurrentComponent((int)BKanban.BInterface.Component_Id);
					if (BKanban.SelectedComponent == NULL)
					{
						Ethernet_GetScheduleInfo(BKanban.BInterface);
					}
					else
					{
						Ethernet_GetScheduleInfo();
					}
				}
				RootNextion.GotoPage(COMPONENT_PAGE);
			}

			if (eop == "BEAM_START_CUTTING")
			{
				BKanban.EthernetRespType = RESP_START_CUTTING;
				RootNextion.showMessage("Start success");
				BKanban.Json_UpdateStartCutting(JsonDoc);
				SaveCuttingInterfaceId();
				Output_Alarm(2);
				BKanban.Cutting.Continue = false;
			}

			if (eop == "BEAM_CONFIRM_SIZE")
			{
				BKanban.EthernetRespType = RESP_CONFIRM_SIZE;
				BKanban.Json_UpdateConfirmSize(JsonDoc);
				Output_Alarm();
				if (BKanban.StopCutting)
					Ethernet_StopCutting();
			}

			if (eop == "BEAM_STOP_CUTTING")
			{
				BKanban.EthernetRespType = RESP_STOP_CUTTING;
				RootNextion.showMessage("Stop success");
				BKanban.Json_UpdateStopCutting(JsonDoc);
				Output_Alarm(3);
				Ethernet_GetLastCut();
			}

			if (eop == "BEAM_GET_LAST_CUT")
			{
				get_last_cut_timeout = false;
				get_lastcut_ok = true;
				BKanban.EthernetRespType = RESP_GET_LAST_CUT;
				BKanban.BInterface.Json_UpdateLastCut(JsonDoc);

				if (BKanban.SelectedSchedule == NULL)
				{
					SelectedSchedule.id = BKanban.BInterface.Schedule_Id;
					SelectedSchedule.OriginalPoId = BKanban.BInterface.OriginalPo_Id;
					BKanban.SelectedSchedule = &SelectedSchedule;
				}

				RootNextion.SetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.START_SEQ, BKanban.BInterface.StartSeq);
				RootNextion.SetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.STOP_SEQ, BKanban.BInterface.StopSeq);
				if (BKanban.BInterface.Finish == false && BKanban.BInterface.BinterfaceId != 0 && !BKanban.Cutting.Continue)
					ButContinueClickCallback();

				RootNextion.GotoPage(BKanban.CurrentWindowId);
			}

			if (eop == "CUT_TIME_RECORD")
			{
				BKanban.EthernetRespType = CUT_TIME_RECORD;
				BKanban.Json_UpdateSubmitCutTime(JsonDoc);
				SaveTotalCutTimes();
				RootNextion.SetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.TOTAL_TIME, BKanban.Cutting.TotalCutTimes);
			}
			if (BKanban.EthernetRespType == RESP_ERROR)
				RootNextion.showMessage("Error occured while requesting to server");
		}
	}
}

void EEPROM_SaveTime(bool isRead)
{
	touch_pad_intr_disable();
	lcal::SaveTime(BKanban.Time.arr, false);
	touch_pad_intr_enable();
}

void EEPROM_SaveMachineInfo(bool isRead)
{
	touch_pad_intr_disable();
	if (isRead)
		lcal::ReadMachineInfo(BKanban.Machine.id, BKanban.Machine.MachineCode, BKanban.Machine.MachineName, sizeof(BKanban.Machine.MachineName));
	else
		lcal::SaveMachineInfo(BKanban.Machine.id, BKanban.Machine.MachineCode, BKanban.Machine.MachineName, sizeof(BKanban.Machine.MachineName));
	touch_pad_intr_enable();
}

void EEPROM_SaveCuttingInfo(bool isRead)
{
	touch_pad_intr_disable();
	f_log();
	EEPROM.begin(512);
	if (isRead)
	{
		quick_log(Read cutting parameters);
		// save interfaceId
		BKanban.Backup.BInterfaceId = lcal::read<int>(SAVE_INTERFACE_ID_ADDR);
	}
	else
	{
		quick_log(Write cutting parameters);
		// save interfaceId
		lcal::write(BKanban.Backup.BInterfaceId, SAVE_INTERFACE_ID_ADDR);
	}
	EEPROM.end();
	touch_pad_intr_enable();
}

void EEPROM_SaveServerInfo(bool isRead)
{
	touch_pad_intr_disable();
	if (isRead)
		lcal::ReadServerInformation(RootEthernet.server, sizeof(RootEthernet.server), RootEthernet.port);
	else
		lcal::SaveServerInformation(RootEthernet.server, sizeof(RootEthernet.server), RootEthernet.port);

	touch_pad_intr_enable();
}

void EEPROM_SaveMac(bool isRead, byte *buf = NULL)
{
	f_log();
	EEPROM.begin(512);
	touch_pad_intr_disable();
	log("MACHINE MAC: ");
	for (int i = 0; i < 6; i++)
	{
		if (isRead)
		{
			*buf = lcal::read<byte>(SAVE_MAC_ADDR + i);
			Serial.print((int)*buf, HEX);
			log(" ");
			buf++;
		}
		else
		{
			lcal::write<byte>(*buf, SAVE_MAC_ADDR + i);
			Serial.print((int)*buf, HEX);
			log(" ");
			buf++;
		}
	}
	EEPROM.end();
	touch_pad_intr_enable();
}

void PAGE_LOADING_EVENT_CALLBACK(uint8_t _pageId, uint8_t _componentId, uint8_t _eventType)
{
	f_log();
	log("PageId: ");
	log_ln(_pageId);
	log("ComponentId: ");
	log_ln(_componentId);
	log("Event type: ");
	log_ln(_eventType);

	switch (_componentId)
	{
	case 200:
		//New page loading
		BKanban.NewPageLoading = true;
		BKanban.CurrentPageId = _pageId;
		break;

	default: // New button event
		switch (_pageId)
		{
		case HIS_PAGE:
			if (_componentId == RootNextion.HisPageHandle.BUT_CONTINUE_ID)
				ButContinueClickCallback();
			if (_componentId == RootNextion.HisPageHandle.BUT_GOT_MACHINE_ID)
				ButSettingMachineClickCallback();
			if (_componentId == RootNextion.HisPageHandle.BUT_GO_SEARCH_ID)
				ButGoSearchClickCallBack();
			if (_componentId == RootNextion.HisPageHandle.BUT_GO_USER_ID)
				ButSettingUserClickCallback();
			break;

		case SEARCH_PAGE:
			if (_componentId == RootNextion.SearchPageHandle.BUT_SEARCHING_ID)
				ButSearchingClickCallback();
			if (_componentId >= RootNextion.SearchPageHandle.FIRST_SELECT_PO_BUT_ID && _componentId < RootNextion.SearchPageHandle.LAST_SELECT_PO_BUT_ID)
				ButSelectScheduleCallback(_componentId);
			if (_componentId == RootNextion.SearchPageHandle.GOT_BACK)
				ButSearchGobackClickCallback();

			break;

		case INFO_PAGE:
			if (_componentId == RootNextion.InfoPageHandle.BUT_NEXT_ID)
				ButInfoGoNextClickCallback();
			if (_componentId == RootNextion.InfoPageHandle.BUT_BACK_ID)
				ButInfoGoBackClickCallback();
			break;

		case CUTTING_PAGE:
			if (_componentId == RootNextion.CuttingPageHandle.BUT_START_ID)
				ButStartCuttingClickCallback();
			if (_componentId == RootNextion.CuttingPageHandle.BUT_GET_SIZE)
				ButGetSizeClickCallback();
			if (_componentId == RootNextion.CuttingPageHandle.BUT_STOP_ID)
				ButStopCuttingClickCallback();
			if (_componentId >= RootNextion.CuttingPageHandle.FIRST_SELECT_SIZE_BUT_ID && _componentId < RootNextion.CuttingPageHandle.LAST_SIZE)
				ButSelectSizeClickCallback(_componentId);
			if (_componentId == RootNextion.CuttingPageHandle.BUT_GO_BACK)
				ButCuttingBackClickCallback();
			break;

		case CONFIRM_SIZE_PAGE:
			if (_componentId == RootNextion.ConfirmSizeHandle.BUT_FINISH_ID)
				ButConfirmFinishSize();
			if (_componentId == RootNextion.ConfirmSizeHandle.BUT_NOT_YET_ID)
				ButConfirmNotYetFinishSize();
			break;

		case MACHINE_PAGE:
			if (_componentId == RootNextion.SettingMachinePageHandle.BUT_SAVE_SERVER_INFO_ID)
				ButSaveServerClickCallback();
			if (_componentId == RootNextion.SettingMachinePageHandle.BUT_SAVE_MACHINE_INFO_ID)
				ButSaveMachineClickCallback();
			if (_componentId == RootNextion.SettingMachinePageHandle.BUT_RESET_ID)
				ButResetClickCallback();
			if (_componentId == RootNextion.SettingMachinePageHandle.BUT_SAVE_MAC)
				ButSaveMacClickCallback();
			break;

		case NEW_USER_PAGE:
			if (_componentId == RootNextion.NewUserHandle.BUT_GO_BACK_ID)
				RootNextion.GotoPage(HIS_PAGE);

			if (_componentId == RootNextion.NewUserHandle.BUT_ADD_NEW_ID)
			{
				ButAddNewUserClickCallback();
				AddNewUser = true;
			}

			break;

		case MESSAGE_PAGE:
			if (_componentId == 100)
				WaitingTimeOutCallback();
			break;

		case COMPONENT_PAGE:
			if (_componentId >= RootNextion.ComponentHandle.FIST_SELECT_COMPONENT_ID &&
				_componentId < RootNextion.ComponentHandle.LAST_COMPONENT)
				ButSelectComponentClickCallback(_componentId);

			if (_componentId == RootNextion.ComponentHandle.GO_BACK)
				ButGobackClickCallback();
			break;

		case WAITING_PAGE:
			if (_componentId == 100)
				WaitingTimeOutCallback();
			break;

		case LOGIN_PAGE:
			if (_componentId == RootNextion.LoginPageHandle.BUT_GO_BACK_ID)
				WaitingTimeOutCallback();
			break;
		}
		break;
	}
}
#endif // !_BeamConfig_h
