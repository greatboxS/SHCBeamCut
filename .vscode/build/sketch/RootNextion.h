// RootNextion.h
#ifndef _ROOTNEXTION_h
#define _ROOTNEXTION_h

#include "HardwareSerial.h"
#include "NextionVariableString.h"
#include "NextionVariableNumeric.h"
#include "NextionText.h"
#include "NextionPicture.h"
#include "NextionPage.h"
#include "NextionNumber.h"
#include "NextionButton.h"
#include "Nextion.h"
#include "INextionTouchable.h"
#include "DataConfig.h"
#include <vector>

// Select which serial port using for Nextion Screen
#define USE_DEFAULT_SERIAL false

// define max length of nextion property name
#define PROPERTY_NAME_MAX_LEN 24

#define TOTAL_NEXTION_PAGE 11
#define TOTAL_NEXTION_BUTTON 5
#define TOTAL_NEXTION_NUMBER 50
#define TOTAL_NEXTION_VARIABLE 50
#define TOTAL_NEXTION_TEXT 70
#define TOTAL_NEXTION_POTEXT 20

// Easy array call
#define NexPageNumber(x) PageNumber[x]
#define NexNumber(x) NexNumber[x]
#define NexText(x) NexText[x]
#define NexPo(x) NexPo[x]
#define NexSize(x) NexSize[x]

#define MESSAGE_t "t0"
#define ETHERNET_STATE_t "ETHERNET"
#define SEARCH_MODE_t "SEARCH_MODE"
#define TIME_OUT_EVENT_ID 100
#define MAX_PROPERTY_LENGTH 24
class RootNextionClass
{
protected:
	//HardwareSerial NexSerial = HardwareSerial(NEXTION_RX_PIN, NEXTION_TX_PIN);
	//Nextion Nex = Nextion(NexSerial);
	HardwareSerial *NexSerial;
	Nextion *Nex;
	const char *NexPropertyType[5] = {"txt", "val", "bco", "pic", "pco"};
	uint32_t Baud, Timeout;

public:
	typedef struct FooterDef_t
	{
		const char *DEVICE_NAME = "DEVICE_NAME";
		const char *R0 = "r0"; //hour
		const char *R1 = "r1"; // min
		const char *R2 = "r2"; //sec
		const char *R3 = "r3"; //day
		const char *R4 = "r4"; //month
		const char *R5 = "r5"; //year

		const char *ETHERNET_PIC = "p0";
	};

	typedef struct HisPageDef_t
	{
		const char *PO_NUMBER = "PO_NUMBER";
		const char *PO_QTY = "PO_QTY";
		const char *START_SEQ = "START_SEQ";
		const char *STOP_SEQ = "STOP_SEQ";
		const char *CUTTING_QTY = "CUTTING_QTY";
		const char *CUTTING_TIME = "CUTTING_TIME";
		const char *START_TIME = "START_TIME";
		const char *STOP_TIME = "STOP_TIME";
		const char *DEVICE_NAME = "DEVICE_NAME";
		const char *COMPONENT = "COMPONENT";

		const uint8_t BUT_CONTINUE_ID = 10;
		const uint8_t BUT_GO_SEARCH_ID = 11;
		const uint8_t BUT_GOT_MACHINE_ID = 12;
		const uint8_t BUT_GO_USER_ID = 13;
	};

	typedef struct SearchPageDef_t
	{
		const char *PO_NUMBER = "PO_NUMBER";
		const char *MONTH = "MONTH";
		const char *LINE = "LINE";
		const char *STOP_SEQ = "STOP_SEQ";
		const char *CUTTING_QTY = "CUTTING_QTY";
		const char *CUTTING_TIME = "CUTTING_TIME";
		const char *START_TIME = "START_TIME";
		const char *STOP_TIME = "STOP_TIME";
		const char *DEVICE_NAME = "DEVICE_NAME";
		const char *SEARCH_MODE = "SEARCH_MODE";

		const uint8_t GOT_BACK = 19;
		const uint8_t BUT_SEARCHING_ID = 20;
		const uint8_t FIRST_SELECT_PO_BUT_ID = 21;
		const uint8_t TOTAL_PO = 15;
		const uint8_t LAST_SELECT_PO_BUT_ID = FIRST_SELECT_PO_BUT_ID + TOTAL_PO;
	};

	typedef struct InfoDef_t
	{
		const char *PO_NUMBER = "PO_NUMBER";
		const char *PO_QTY = "PO_QTY";
		const char *PO_LINE = "PO_LINE";
		const char *MODEL_NUMBER = "MODEL_NUMBER";
		const char *MODEL_NAME = "MODEL_NAME";
		const char *ARTICLE = "ARTICLE";

		const uint8_t BUT_NEXT_ID = 30;
		const uint8_t BUT_BACK_ID = 31;
	};

	typedef struct CuttingPageDef_t
	{
		const char *PO_NUMBER = "PO_NUMBER";
		const char *START_SEQ = "START_SEQ";
		const char *STOP_SEQ = "STOP_SEQ";
		const char *TIME = "TIME";
		const char *SEQ_QTY = "SEQ_QTY";
		const char *CUTTING_QTY = "CUTTING_QTY";
		const char *TOTAL_SEQ = "TOTAL_SEQ";
		const char *CUTTING_TIME = "CUTTING_TIME";
		const char *TOTAL_TIME = "TOTAL_TIME";

		const char *COMPONENT = "COMPONENT";

		const char *USER_NAME = "USER_NAME";

		const uint8_t BUT_START_ID = 40;
		const uint8_t BUT_STOP_ID = 41;
		const uint8_t BUT_GET_SIZE = 42;
		const uint8_t BUT_GO_BACK = 39;

		const uint8_t FIRST_SELECT_SIZE_BUT_ID = 43;
		const uint8_t LAST_SIZE = FIRST_SELECT_SIZE_BUT_ID + 30;
	};

	typedef struct ConfirmSizePageDef_t
	{
		const char *INPUT_SIZE = "n4";
		const char *SIZE_QTY = "SIZE_QTY";
		const char *CUT_QTY = "CUT_QTY";
		const char *TOTAL_QTY = "n0";

		const uint8_t BUT_CANCEL_ID = 52;
		const uint8_t BUT_FINISH_ID = 50;
		const uint8_t BUT_NOT_YET_ID = 51;
	};

	typedef struct ComponentDef_t
	{
		const uint8_t FIRST_COMPONENT_NAME_ID = 0;
		const uint8_t FIST_SELECT_COMPONENT_ID = 60;
		const uint8_t GO_BACK = 16;
		const uint8_t LAST_COMPONENT = 24 + FIST_SELECT_COMPONENT_ID;
	};

	typedef struct SettingMachineDef_t
	{
		const char *MACHINE_CODE = "t0";
		const char *MACHINE_NAME = "t1";
		const char *SERVER_IP = "t2";
		const char *SERVER_PORT = "n0";
		const char *LOCAL_IP = "t3";

		const uint8_t BUT_SAVE_MACHINE_INFO_ID = 70;
		const uint8_t BUT_SAVE_SERVER_INFO_ID = 71;
		const uint8_t BUT_RESET_ID = 72;
		const uint8_t BUT_SAVE_MAC = 0x49;
	};

	typedef struct MESSAGEDef_t
	{
		const char *MESSAGE = "t0";
		const uint8_t BUT_BACK_ID = 80;
	};

	typedef struct WaitingPageDef_t
	{
		const uint8_t TIMEOUT_EVENT_ID = 90;
	};

	typedef struct NewUserDef_t
	{
		const char *NEW_USER_NAME = "t0";
		const char *NEW_USER_RFID = "t1";
		const char *USER_ID_NUMBER = "n3";
		const char *NEW_USER_DEP = "t2";

		const uint8_t BUT_ADD_NEW_ID = 100;
		const uint8_t BUT_GO_BACK_ID = 101;
	};

	typedef struct LoginPageDef_t
	{
		const char *NEW_USER_NAME = "t0";
		const char *NEW_USER_RFID = "t1";
		const char *USER_ID_NUMBER = "n3";
		const char *NEW_USER_DEP = "t2";
		const char *JOB_TITLE = "JOB_TILE";

		const uint8_t BUT_SAVE_ID = 110;
		const uint8_t BUT_GO_BACK_ID = 111;
	};

	// RootNextionClass(uint8_t rx_pin, uint8_t tx_pin, uint32_t baud, uint32_t timeout)
	// {
	// 	NexSerial = new HardwareSerial(rx_pin, tx_pin);
	// 	Nex = new Nextion(*NexSerial);
	// 	Baud = baud;
	// 	Timeout = timeout;
	// }

	RootNextionClass(uint8_t num, uint32_t baud, uint32_t timeout)
	{
		NexSerial = new HardwareSerial(num);
		Nex = new Nextion(*NexSerial);
		Baud = baud;
		Timeout = timeout;
	}

	const char *NexNumber[TOTAL_NEXTION_NUMBER];
	const char *NexText[TOTAL_NEXTION_TEXT];
	const char *NexPo[TOTAL_NEXTION_POTEXT];
	const char *NexSize[TOTAL_NEXTION_TEXT];

	const uint8_t PageNumber[TOTAL_NEXTION_PAGE]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	const char *PageName[TOTAL_NEXTION_PAGE]{"HIS_P", "SEARCH_P", "INFO_P",
											 "CUTTING_P", "CONFIRM_SIZE_P", "COMPONENT_P", "MACHINE_P",
											 "MESSAGE_P", "WAITING_P", "NEW_USER_P", "LOGIN_P"};

	INextionTouchable *PAGE_LOADING_EVENT;

	FooterDef_t FooterDef;

	HisPageDef_t HisPageHandle;

	SearchPageDef_t SearchPageHandle;

	InfoDef_t InfoPageHandle;

	CuttingPageDef_t CuttingPageHandle;

	ConfirmSizePageDef_t ConfirmSizeHandle;

	ComponentDef_t ComponentHandle;

	SettingMachineDef_t SettingMachinePageHandle;

	MESSAGEDef_t MessageBoxPageHandle;

	WaitingPageDef_t WaitingPageHandle;

	NewUserDef_t NewUserHandle;

	LoginPageDef_t LoginPageHandle;

	void init();

	void GotoPage(uint8_t _page)
	{
		Serial.print("Goto page:");
		Serial.println(_page);
		size_t commandLen = 6 + strlen(PageName[_page]);
		char command[commandLen];
		snprintf(command, commandLen, "page %s", PageName[_page]);
		sendCommand(command);
	}

	void showMessage(char *_message, int wait_time = 3000)
	{
		SetPage_stringValue(MESSAGE_PAGE, MESSAGE_t, _message);
		GotoPage(MESSAGE_PAGE);
		setNumberProperty(PageName[MESSAGE_PAGE], "tm0.tim", wait_time);
	}

	void showMessage(const char *_message, int wait_time = 3000)
	{
		char message[128]{0};
		memccpy(message, _message, 0, sizeof(message));
		SetPage_stringValue(MESSAGE_PAGE, MESSAGE_t, message);
		GotoPage(MESSAGE_PAGE);
		setNumberProperty(PageName[MESSAGE_PAGE], "tm0.tim", wait_time);
	}

	void setLanStatus(uint8_t pageId, bool _status) // false = disconnected
	{
		if (_status)
		{
			setNumberProperty(PageName[pageId], "p0.pic", 10);
		}
		else
		{
			setNumberProperty(PageName[pageId], "p0.pic", 11);
		}
		SetPage_numberValue(LOGIN_PAGE, ETHERNET_STATE_t, (uint32_t)_status);
	}

	uint8_t get_PageNumber()
	{
		return Nex->getCurrentPage();
	}

	void Reset() { sendCommand("rest"); }
	//----------------------------------------------------------------------------------------------------------------

	void Waiting(int wait_time = 10000, char *mesg = "")
	{
		GotoPage(WAITING_PAGE);
		setNumberProperty(PageName[WAITING_PAGE], "tm0.tim", wait_time);
		setStringProperty(PageName[WAITING_PAGE], "t0.txt", mesg);
	}

	// Listen to Nextion device
	void Listening() { Nex->poll(); }

	void SetPage_stringValue(BKanbanPage_t page, const char *propertyName, const char *str);
	void SetPage_stringValue(BKanbanPage_t page, const char *propertyName, char *str);

	void SetPage_numberValue(BKanbanPage_t page, const char *propertyName, uint32_t value);

	uint32_t GetPage_numberValue(BKanbanPage_t page, const char *propertyName);
	void GetPage_stringValue(BKanbanPage_t page, const char *propertyName, char *outputStr, size_t len);

	void SetPage_propertyBackgroundColor(BKanbanPage_t page, const char *propertyName, uint32_t color);
	void SetPage_propertyForceColor(BKanbanPage_t page, const char *propertyName, uint32_t color);

	void setPage_stringAsNumberProperty(uint8_t page, const char *propertyName, uint32_t value);

	void setPage_stringAsNumberProperty(uint8_t page, char *propertyName, uint32_t value);

	void sendCommand(char *commandStr);
	/*!
 * \brief Sets the value of a numerical property of this widget.
 * \param propertyName Name of the property
 * \param value Value
 * \return True if successful
 */
	void setNumberProperty(const char *pageName, char *propertyName, uint32_t value);
	/*!
	 * \brief Gets the value of a numerical property of this widget.
	 * \param propertyName Name of the property
	 * \return Value (may also return 0 in case of error)
	 */
	uint32_t getNumberProperty(const char *pageName, char *propertyName);
	/*!
	 * \brief Sets the value of a string property of this widget.
	 * \param propertyName Name of the property
	 * \param value Value
	 * \return True if successful
	 */
	void setStringProperty(const char *pageName, char *propertyName, char *value);
	/*!
	 * \brief Gets the value of a string property of this widget.
	 * \param propertyName Name of the property
	 * \param value Pointer to char array to store result in
	 * \param len Maximum length of value
	 * \return Actual length of value
	 */
	size_t getStringProperty(const char *pageName, char *propertyName, char *value,
							 size_t len);

	void setNumberProperty(const char *propertyName, uint32_t value);

	uint32_t getNumberProperty(const char *propertyName);

	void setStringProperty(const char *propertyName, char *value);

	size_t getStringProperty(const char *propertyName, char *value, size_t len);

	void getPropertyName(const char *inputName, char *outputName, uint8_t type)
	{
		uint8_t propertyNameLen = strlen(inputName) + MAX_PROPERTY_LENGTH;
		snprintf(outputName, propertyNameLen, "%s.%s", inputName, NexPropertyType[type]);
	}
};

#endif // !_ROOTNEXTION_h
