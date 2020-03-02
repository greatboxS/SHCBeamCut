#ifndef _DataConfig_h
#define _DataConfig_h

#include "JsonGetMember.h"
#include "ArduinoJson.h"
#include <vector>
#include <queue>

#define quick_log(...) Serial.print(#__VA_ARGS__)
#define log(...) Serial.print(__VA_ARGS__)
#define log_ln(...) Serial.println(__VA_ARGS__)
#define f_log() Serial.println(__func__)

#define NEXTION_SERIAL_TIMEOUT 200
#define NEXTION_SERIAL_BAUD 9600

typedef enum BKanbanPage_t
{
	HIS_PAGE,
	SEARCH_PAGE,
	INFO_PAGE,
	CUTTING_PAGE,
	CONFIRM_SIZE_PAGE,
	COMPONENT_PAGE,
	MACHINE_PAGE,
	MESSAGE_PAGE,
	WAITING_PAGE,
	NEW_USER_PAGE,
	LOGIN_PAGE
};

typedef enum NextionPropertyType_t
{
	NEX_TEXT,
	NEX_NUMBER,
	NEX_BACKGROUND_COLOR,
	NEX_PICTURE,
	NEX_FORCE_COLOR,
};

typedef enum RespExpType_t
{
	RESP_UNKNOW,
	RESP_GET_TIME,
	RESP_GET_SCHEDULE,
	RESP_GET_SCHEDULE_INFO,
	RESP_GET_SIZE,
	RESP_GET_COMPONENT,
	RESP_START_CUTTING,
	RESP_STOP_CUTTING,
	RESP_CONFIRM_SIZE,
	RESP_GET_USER,
	RESP_GET_LAST_CUT,
	RESP_GET_MACHINE,
	CUT_TIME_RECORD,
	RESP_ERROR,

};

typedef struct
{
	char code[32]; //code
	int dataLength;
	char buf[1024 * 3];
} HttpHeader_t;

typedef struct User_t
{
	long id;
	bool Pass;
	int UserCode;
	char Division[5];
	char UserName[50];
	char UserRFID[10];
	String Department;
	String JobTitle;
	void SetRFID(char *rfid)
	{
		if (rfid != NULL)
			memccpy(UserRFID, rfid, 0, sizeof(UserRFID));
	}

	void SetRFID(String &rfid)
	{
		if (rfid.c_str() != NULL)
			memccpy(UserRFID, rfid.c_str(), 0, sizeof(UserRFID));
	}

	void SetName(char *name)
	{
		if (name != NULL)
			memccpy(UserRFID, name, 0, sizeof(UserRFID));
	}

	void SetName(String &name)
	{
		if (name.c_str() != NULL)
			memccpy(UserRFID, name.c_str(), 0, sizeof(UserRFID));
	}

	void SetProfile(int _id, int _UserCode, char *name, char *rfid)
	{
		if (id != 0)
			id = _id;

		if (_UserCode != 0)
			UserCode = _UserCode;

		SetName(name);

		SetRFID(rfid);
	}

	void UpdateUser(JsonDocument &JsonDoc)
	{
		f_log();
		JsonParse_Element(JsonDoc, "id", id);
		JsonParse_Element(JsonDoc, "Pass", Pass);
		JsonParse_Element(JsonDoc, "UserCode", UserCode);
		JsonParse_Element(JsonDoc, "UserName", UserName, sizeof(UserName));
		JsonParse_Element(JsonDoc, "UserRFID", UserRFID, sizeof(UserRFID));
		JsonParse_Element(JsonDoc, "Department", Department);
		JsonParse_Element(JsonDoc, "JobTitle", JobTitle);
	}
};

typedef struct
{
	char startTime[50];
	char stopTime[50];
} CuttingTime_t;

typedef struct DateTime_t
{
	void SetTime(uint8_t day, uint8_t month, uint8_t year, uint8_t hour, uint8_t min, uint8_t sec)
	{
		arr[0] = day;
		arr[1] = month;
		arr[2] = year;
		arr[3] = hour;
		arr[4] = min;
		arr[5] = sec;
	}

	void SetTime(uint8_t *_arr)
	{
		for (size_t i = 0; i < 6; i++)
		{
			arr[i] = *_arr;
			_arr++;
		}
	}

	void Print()
	{
		char temp[50]{0};
		snprintf(temp, sizeof(temp), "DateTime: %d: %d: %d, %d/ %d/ %d", arr[3], arr[4], arr[5], arr[0], arr[1], arr[2]);
		log_ln(temp);
	}

	uint8_t arr[6];
	uint8_t &Day() { return arr[0]; }
	uint8_t &Month() { return arr[1]; }
	uint8_t &Year() { return arr[2]; }
	uint8_t &Hour() { return arr[3]; }
	uint8_t &Min() { return arr[4]; }
	uint8_t &Sec() { return arr[5]; }
};

// Error
typedef struct
{
	String Request;
	char Message[100];
} Error_t;

typedef struct SysFlag_t
{
	bool IsRequest;
	bool TagFlag;
	bool IsCutting;
	bool IsConfirmSize;
	bool SelectHis;
	bool RespError = false;
};

typedef struct Part_t
{
	long id;
	char Name[30];
};

typedef struct Size_t
{
	uint8_t SizeId;
	uint16_t SizeQty;
	uint16_t CuttingQty;
	uint16_t WorkerConfirmQty;
	bool Finish;
};

typedef struct Schedule_t
{
	long id;
	char PoNumber[20];
	int OriginalPoId = 0;
	int Quantity;
	bool Cutting;
	bool Finish;
};

typedef struct Machine_t
{
	int id;
	char MachineName[32];
	char MachineCode[32];
	void Json_UpdateMachineInfo(JsonDocument JsonDoc)
	{
		f_log();
		JsonParse_Element(JsonDoc, "id", id);
		JsonParse_Element(JsonDoc, "Name", MachineName, sizeof(MachineName));
		JsonParse_Element(JsonDoc, "SystemCode", MachineCode, sizeof(MachineCode));
	}
};

typedef struct Backup_t
{
	int BInterfaceId;
	int ClonePoId;
	int CloneSeqId;
	int ComponentId;
};

typedef struct Sequence_t
{
	long id;
	int SeqNo;
	int SeqQty;
	int CuttingQty;
	bool Finish = false;
};

typedef struct ScheduleInfo_t
{
	long id;
	char PoNumber[20];
	int Quantity;
	char Line[5];
	char Model[10];
	char ModelName[50];
	char Article[10];
	int ClonePoId;

	std::vector<Sequence_t> SeqList;

	void clear()
	{
		SeqList.clear();
		id = 0;
	}
};

typedef struct BInterface_t
{
	int BinterfaceId;
	String PoNumber;
	String StartTime;
	String StopTime;
	String Component;
	int OriginalPo_Id;
	int PoQty;
	int CutQty;
	int CutTime;
	int Employee_Id;
	int Schedule_Id;
	int BeamCutPo_Id;
	int Component_Id;
	int BeamCutStartSeq_Id;
	int BeamCutStopSeq_Id;
	int StartSeq;
	int StopSeq;
	bool Finish;

	void Json_UpdateLastCut(JsonDocument &JsonDoc)
	{
		JsonParse_Element(JsonDoc, "BinterfaceId", BinterfaceId);
		JsonParse_Element(JsonDoc, "OriginalPo_Id", OriginalPo_Id);
		JsonParse_Element(JsonDoc, "PoQty", PoQty);
		JsonParse_Element(JsonDoc, "PoNumber", PoNumber);
		JsonParse_Element(JsonDoc, "StartTime", StartTime);
		JsonParse_Element(JsonDoc, "StopTime", StopTime);
		JsonParse_Element(JsonDoc, "Component", Component);
		JsonParse_Element(JsonDoc, "CutQty", CutQty);
		JsonParse_Element(JsonDoc, "CutTime", CutTime);
		JsonParse_Element(JsonDoc, "Employee_Id", Employee_Id);
		JsonParse_Element(JsonDoc, "Schedule_Id", Schedule_Id);
		JsonParse_Element(JsonDoc, "BeamCutPo_Id", BeamCutPo_Id);
		JsonParse_Element(JsonDoc, "Component_Id", Component_Id);
		JsonParse_Element(JsonDoc, "BeamCutStartSeq_Id", BeamCutStartSeq_Id);
		JsonParse_Element(JsonDoc, "BeamCutStopSeq_Id", BeamCutStopSeq_Id);
		JsonParse_Element(JsonDoc, "StartSeq", StartSeq);
		JsonParse_Element(JsonDoc, "StopSeq", StopSeq);
		JsonParse_Element(JsonDoc, "Finish", Finish);
	}

	void clear()
	{
		BinterfaceId = 0;
		Finish = false;
	}
};

typedef struct Cutting_t
{
	int TotalSelectedQty = 0;
	int TotalCuttingQty = 0;
	int TotalCutTimes = 0;
	int CurrentCutTimes = 0;
	int SubmitCutTime = 0;
	int OldSubmitCutTime = 0;
	int RunTime = 0;
	uint8_t sec = 0;

	int RespTotalCutQty;
	int REspTotalCutTime;

	bool Continue = false;
	bool IsCutting = false;
	bool StartCuttingSuccess = false;
	bool StopCuttingSuccess = false;

	int StartScheduleId = 0;
	std::vector<Schedule_t> ScheduleList;
	std::vector<Size_t> SizeList;
	std::vector<Part_t> ComponentList;
	ScheduleInfo_t ScheduleInfo;
	User_t Worker;

	Schedule_t *SelectCurrentSchedule(uint8_t index)
	{
		if (index < ScheduleList.size() && ScheduleList.size() > 0)
		{
			SizeList.clear();
			ScheduleInfo.clear();
			return &ScheduleList.at(index);
		}
		else
			return NULL;
	}

	Sequence_t *SelectCurrentSequenece(uint8_t index)
	{
		if (index < ScheduleInfo.SeqList.size() && ScheduleInfo.SeqList.size() > 0)
			return &ScheduleInfo.SeqList.at(index);
		else
			return NULL;
	}

	Size_t *SelectCurrentSize(uint8_t sizeID)
	{
		for (size_t i = 0; i < SizeList.size(); i++)
		{
			if (SizeList[i].SizeId == sizeID)
				return &SizeList.at(i);
		}
		return NULL;
	}

	Part_t *SelectCurrentComponent(uint8_t index)
	{
		if (index < ComponentList.size() && ComponentList.size() > 0)
		{
			ScheduleInfo.clear();
			SizeList.clear();
			return &ComponentList.at(index);
		}
		else
			return NULL;
	}

	Part_t *AutoSelectCurrentComponent(int id)
	{
		for(int i=0;i< ComponentList.size() ;i++)
		{
			if(ComponentList.at(i).id==id)
			{
				return &ComponentList.at(i);
			}
		}
		return NULL;
	}

	void Json_UpdateScheduleList(JsonDocument JsonDoc)
	{
		f_log();
		ScheduleList.clear();
		ScheduleInfo.clear();
		StartScheduleId = 0;
		JsonArray JsonArr = JsonDoc.getMember("ScheduleList");
		for (size_t i = 0; i < JsonArr.size(); i++)
		{
			Schedule_t schedule;
			JsonParse_Element(JsonArr, i, "id", schedule.id);
			JsonParse_Element(JsonArr, i, "PoNumber", schedule.PoNumber, sizeof(ScheduleList.at(i).PoNumber));
			JsonParse_Element(JsonArr, i, "PoQty", schedule.Quantity);
			JsonParse_Element(JsonArr, i, "Cutting", schedule.Cutting);
			JsonParse_Element(JsonArr, i, "OriginalPoId", schedule.OriginalPoId);
			ScheduleList.emplace_back(schedule);
			StartScheduleId = schedule.id;
		}
	}

	void Json_UpdateScheduleInfo(JsonDocument JsonDoc)
	{
		f_log();
		ScheduleInfo.clear();
		JsonParse_Element(JsonDoc, "ClonePoId", ScheduleInfo.ClonePoId);

		JsonObject JsonObj = JsonDoc.getMember("PoInfo");
		JsonParse_Element(JsonObj, "id", ScheduleInfo.id);
		JsonParse_Element(JsonObj, "PoNumber", ScheduleInfo.PoNumber, sizeof(ScheduleInfo.PoNumber));
		JsonParse_Element(JsonObj, "Quantity", ScheduleInfo.Quantity);
		JsonParse_Element(JsonObj, "Line", ScheduleInfo.Line, sizeof(ScheduleInfo.Line));
		JsonParse_Element(JsonObj, "Model", ScheduleInfo.Model, sizeof(ScheduleInfo.Model));
		JsonParse_Element(JsonObj, "ModelName", ScheduleInfo.ModelName, sizeof(ScheduleInfo.ModelName));
		JsonParse_Element(JsonObj, "Article", ScheduleInfo.Article, sizeof(ScheduleInfo.Article));

		JsonArray JsonArr = JsonDoc.getMember("SeqList");
		for (size_t i = 0; i < JsonArr.size(); i++)
		{
			Sequence_t seq;
			JsonParse_Element(JsonArr, i, "id", seq.id);
			JsonParse_Element(JsonArr, i, "SeqNo", seq.SeqNo);
			JsonParse_Element(JsonArr, i, "SeqQty", seq.SeqQty);
			JsonParse_Element(JsonArr, i, "CuttingQty", seq.CuttingQty);
			JsonParse_Element(JsonArr, i, "Finish", seq.Finish);
			ScheduleInfo.SeqList.emplace_back(seq);
		}
	}

	void Json_UpdateSizeList(JsonDocument JsonDoc)
	{
		f_log();
		SizeList.clear();
		JsonParse_Element(JsonDoc, "RespTotalQty", TotalSelectedQty);
		JsonParse_Element(JsonDoc, "RespTotalCuttingQty", TotalCuttingQty);
		JsonArray JsonArr = JsonDoc.getMember("SizeList");
		for (size_t i = 0; i < JsonArr.size(); i++)
		{
			Size_t size;
			JsonParse_Element(JsonArr, i, "SizeId", size.SizeId);
			JsonParse_Element(JsonArr, i, "SizeQty", size.SizeQty);
			JsonParse_Element(JsonArr, i, "CuttingQty", size.CuttingQty);
			JsonParse_Element(JsonArr, i, "Finish", size.Finish);
			SizeList.emplace_back(size);
		}
	}

	void Json_UpdateComponentList(JsonDocument JsonDoc)
	{
		f_log();
		ComponentList.clear();
		JsonArray JsonArr = JsonDoc.getMember("ComponentList");
		for (size_t i = 0; i < JsonArr.size(); i++)
		{
			Part_t part;
			JsonParse_Element(JsonArr, i, "id", part.id);
			JsonParse_Element(JsonArr, i, "Name", part.Name, sizeof(part.Name));
			ComponentList.emplace_back(part);
		}
	}

	void clear()
	{
		ScheduleList.clear();
		SizeList.clear();
		ScheduleInfo.clear();
		ComponentList.clear();

		TotalSelectedQty = 0;
		TotalCuttingQty = 0;
		TotalCutTimes = 0;
		CurrentCutTimes = 0;
		RunTime = 0;

		RespTotalCutQty = 0;
		REspTotalCutTime = 0;

		IsCutting = false;
		StartCuttingSuccess = false;
		StopCuttingSuccess = false;
	}
};

typedef struct BKanban_t
{
	bool NewPageLoading = false;
	bool SelectSequencePass = false;
	uint8_t EthernetState;

	uint8_t SearchMode = 1;
	DateTime_t Time;
	Machine_t Machine;
	Cutting_t Cutting;
	BInterface_t BInterface;
	Backup_t Backup;

	bool StopCutting = false;
	bool GotoNextPage = false;
	uint8_t NextPageId;
	uint8_t CurrentPageId;
	uint8_t CurrentWindowId = 0;

	Schedule_t *SelectedSchedule;
	Sequence_t *SelectedStartSequence;
	Sequence_t *SelectedStopSequence;
	Size_t *SelectedSize;
	Part_t *SelectedComponent;

	RespExpType_t EthernetRespType = RESP_UNKNOW;

	uint8_t TotalSequence()
	{
		return Cutting.ScheduleInfo.SeqList.size();
	}

	uint8_t TotalSchedule()
	{
		return Cutting.ScheduleList.size();
	}

	uint8_t TotalComponent()
	{
		return Cutting.ComponentList.size();
	}

	uint8_t TotalSize()
	{
		return Cutting.SizeList.size();
	}

	void Json_UpdateStartCutting(JsonDocument JsonDoc)
	{
		f_log();
		Cutting.CurrentCutTimes = 0;
		Cutting.RunTime = 0;
		Cutting.sec = 0;
		Cutting.TotalCuttingQty = 0;
		Cutting.IsCutting = true;
		JsonParse_Element(JsonDoc, "InterfaceId", BInterface.BinterfaceId);
	}

	void Json_UpdateStopCutting(JsonDocument JsonDoc)
	{
		f_log();
		Cutting.IsCutting = false;
		Cutting.Continue = false;
	}

	void Json_UpdateConfirmSize(JsonDocument JsonDoc)
	{
		f_log();
		JsonParse_Element(JsonDoc, "CutQty", SelectedSize->CuttingQty);
		JsonParse_Element(JsonDoc, "Finish", SelectedSize->Finish);
		JsonParse_Element(JsonDoc, "RespTotalCuttingQty", Cutting.TotalCuttingQty);
		StopCutting = false;
		JsonParse_Element(JsonDoc, "StopCutting", StopCutting);
	}

	void Json_UpdateTime(JsonDocument JsonDoc)
	{
		f_log();
		JsonParse_Element(JsonDoc, "Day", Time.arr[0]);
		JsonParse_Element(JsonDoc, "Month", Time.arr[1]);
		JsonParse_Element(JsonDoc, "Year", Time.arr[2]);
		JsonParse_Element(JsonDoc, "Hour", Time.arr[3]);
		JsonParse_Element(JsonDoc, "Min", Time.arr[4]);
		JsonParse_Element(JsonDoc, "Sec", Time.arr[5]);
		//lcal::SaveTime(Time.arr, false);
	}

	void Json_UpdateSubmitCutTime(JsonDocument JsonDoc)
	{
		f_log();
		JsonParse_Element(JsonDoc, "TotalCutTime", Cutting.TotalCutTimes);
		if (Cutting.SubmitCutTime >= Cutting.OldSubmitCutTime)
			Cutting.SubmitCutTime -= Cutting.OldSubmitCutTime;
		else
			Cutting.OldSubmitCutTime = 0;
	}
};

typedef struct Request_t
{
	void Update(char *_url, bool _show)
	{
		memccpy(url, _url, 0, sizeof(url));
		ShowWaitingPage = _show;
	}
	char url[64]{0};
	uint8_t retry = 0;
	bool Cancel = false;
	bool RetryNew()
	{
		retry++;
		if (retry >= 3)
		{
			Cancel = true;
		}
		return Cancel;
	}
	bool ShowWaitingPage = false;
};

typedef struct RequestQueue_t
{
	std::queue<Request_t> List;
	
	void retry_failed()
	{
		if(List.front().RetryNew())
		{
			List.pop();
		}
	}
};

#endif // !_DataConfig_h
