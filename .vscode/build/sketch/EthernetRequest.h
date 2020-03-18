#ifndef _EthernetRequest_h
#define _EthernetRequest_h

void SendRequest(char *url, bool showWaitingPage);

void Ethernet_GetSchedule();
void Ethernet_GetScheduleInfo();
void Ethernet_GetSize();
void Ethernet_GetTime();
void Ethernet_GetHitory(uint32_t interfaceId);
void Ethernet_StartCutting();
void Ethernet_StopCutting();
void Ethernet_ConfirmSize(uint32_t sizeQty);
void Ethernet_GetComponent();
void Ethernet_GetWorkerInfo();
void Ethernet_GetLastCut();
void Ethernet_GetScheduleInfo(BInterface_t &binterface);
void Ethernet_GetSize(BInterface_t &binterface);
void Ethernet_StartCutting(BInterface_t &binterface);
void Ethernet_SubmitCuttingTime();
void Ethernet_RequestQueue();
////--------------------------------------------------------------------------
void Ethernet_GetSchedule()
{
	f_log();
	char line[5]{0};
	char poNumber[20]{0};
	char url[64]{0};
	RootNextion.GetPage_stringValue(SEARCH_PAGE, RootNextion.SearchPageHandle.PO_NUMBER, poNumber, sizeof(poNumber));
	RootNextion.GetPage_stringValue(SEARCH_PAGE, RootNextion.SearchPageHandle.LINE, line, sizeof(line));
	uint8_t mode = RootNextion.GetPage_numberValue(SEARCH_PAGE, RootNextion.SearchPageHandle.SEARCH_MODE);
	int month = RootNextion.GetPage_numberValue(SEARCH_PAGE, RootNextion.SearchPageHandle.MONTH);

	if (BKanban.SearchMode > 3 && BKanban.SearchMode < 1)
		BKanban.SearchMode = 1;
	// mode 1 search
	// mode 2 get from schedule
	// mode 3 get from order
	Serial.println("Mode");
	Serial.println(mode);
	switch (mode)
	{
	case 1:
		if (strlen(poNumber) == 0)
		{
			poNumber[0] = ' ';
		}
		snprintf(url, sizeof(url), "api-search_schedule/%s/%d/%s", line, month, poNumber);
		break;

	case 2:
		snprintf(url, sizeof(url), "api-get_schedule/%s/%d/%d", line, month, BKanban.Cutting.StartScheduleId);
		break;

	case 3:
		snprintf(url, sizeof(url), "api-get_order/%d/%d", BKanban.Machine.id, BKanban.Cutting.Worker.id);
		break;

	default:
		RootNextion.showMessage("Invalid search mode");
		return;
		break;
	}

	SendRequest(url, true);
}

void Ethernet_GetScheduleInfo()
{
	f_log();
	if (BKanban.SelectedSchedule == NULL)
	{
		RootNextion.showMessage("Please select schedule first!");
		return;
	}

	if (BKanban.SelectedComponent == NULL)
	{
		RootNextion.showMessage("Please select component first!");
		return;
	}

	char url[64]{0};
	snprintf(url, sizeof(url), "api-get_scheduleInfo/%d/%d/%d",
			 BKanban.SelectedSchedule->id, BKanban.SelectedSchedule->OriginalPoId, BKanban.SelectedComponent->id);
	SendRequest(url, true);
}

void Ethernet_GetSize()
{
	f_log();
	uint8_t startSeq = RootNextion.GetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.START_SEQ);
	delay(10);
	uint8_t stopSeq = RootNextion.GetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.STOP_SEQ);

	uint8_t total = BKanban.TotalSequence();

	BKanban.SelectSequencePass = true;
	if (startSeq > stopSeq ||
		startSeq > total ||
		stopSeq > total ||
		startSeq == 0 ||
		stopSeq == 0)
	{
		BKanban.SelectSequencePass = false;
		RootNextion.showMessage("Invalid sequence number");
		return;
	}

	BKanban.SelectedStartSequence = BKanban.Cutting.SelectCurrentSequenece(startSeq - 1);
	BKanban.SelectedStopSequence = BKanban.Cutting.SelectCurrentSequenece(stopSeq - 1);
	char url[64]{0};
	snprintf(url, sizeof(url), "api-get_size/%d/%d", BKanban.SelectedStartSequence->id, BKanban.SelectedStopSequence->id);
	SendRequest(url, true);
}

void Ethernet_GetTime()
{
	f_log();
	SendRequest("api-gettime", false);
}

void Ethernet_GetHitory(uint32_t interfaceId)
{
	f_log();
	char url[64]{0};
	snprintf(url, sizeof(url), "api-get_history/%d", interfaceId);
	SendRequest(url, true);
}

void Ethernet_StartCutting()
{
	f_log();

	// uint8_t startSeq = RootNextion.GetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.START_SEQ);
	// delay(10);
	// uint8_t stopSeq = RootNextion.GetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.STOP_SEQ);

	// uint8_t total = BKanban.TotalSequence();

	// BKanban.SelectSequencePass = true;
	// if (startSeq > stopSeq ||
	// 	startSeq > total ||
	// 	stopSeq > total ||
	// 	startSeq == 0 ||
	// 	stopSeq == 0)
	// {
	// 	BKanban.SelectSequencePass = false;
	// 	RootNextion.showMessage("Invalid sequence number");
	// 	return;
	// }

	// BKanban.SelectedStartSequence = BKanban.Cutting.SelectCurrentSequenece(startSeq - 1);
	// BKanban.SelectedStopSequence = BKanban.Cutting.SelectCurrentSequenece(stopSeq - 1);

	// if (BKanban.SelectedStartSequence == NULL ||
	// 	BKanban.SelectedStopSequence == NULL)
	// {
	// 	RootNextion.showMessage("Please select sequence!");
	// 	return;
	// }

	if (BKanban.Cutting.IsCutting)
	{
		//RootNextion.showMessage("You are cutting");
		return;
	}

	if (BKanban.Cutting.Worker.id == 0)
	{
		RootNextion.showMessage("Please check tag again!");
		return;
	}

	if (BKanban.Machine.id == 0)
	{
		RootNextion.showMessage("Please config beam cut kanban first!");
		return;
	}

	if (BKanban.SelectedComponent == NULL || BKanban.SelectedComponent->id == 0)
	{
		RootNextion.showMessage("Please select component!");
		return;
	}

	///api-startcutting/{deviceId}/{workerId}/{scheduleId}/{clonePoId}/{seq1Id}/{seq2Id}
	char url[64]{0};
	snprintf(url, sizeof(url), "api-start_cutting/%d/%d/%d/%d/%d/0", BKanban.Machine.id, BKanban.Cutting.Worker.id,
			 BKanban.Cutting.ScheduleInfo.ClonePoId, BKanban.SelectedStartSequence->id, BKanban.SelectedStopSequence->id);

	SendRequest(url, true);
}

void Ethernet_StopCutting()
{
	f_log();

	if (!BKanban.Cutting.IsCutting)
	{
		RootNextion.showMessage("You are stop!");
		return;
	}

	///api-startcutting/{deviceId}/{workerId}/{scheduleId}/{clonePoId}/{seq1Id}/{seq2Id}
	char url[64]{0};
	snprintf(url, sizeof(url), "api-stop_cutting/%d/%d", BKanban.BInterface.BinterfaceId, BKanban.Cutting.CurrentCutTimes);
	SendRequest(url, true);
}

void Ethernet_ConfirmSize(uint32_t sizeQty)
{
	f_log();
	if (BKanban.SelectedSize == NULL)
	{
		RootNextion.showMessage("Please select size first!");
		return;
	}

	if (BKanban.SelectedSize->Finish)
	{
		RootNextion.showMessage("This size is already confirm!");
		return;
	}

	if (sizeQty == 0)
	{
		RootNextion.showMessage("Please key in size quantity!");
		return;
	}

	char url[64]{0};
	snprintf(url, sizeof(url), "api-confirm_size/%d/%d/%d", BKanban.BInterface.BinterfaceId, BKanban.SelectedSize->SizeId, sizeQty);
	SendRequest(url, true);
}

void Ethernet_GetComponent()
{
	f_log();
	char url[64]{0};
	snprintf(url, sizeof(url), "api-get_component/%d/%d",
			 BKanban.SelectedSchedule->id, BKanban.SelectedSchedule->OriginalPoId);
	SendRequest(url, true);
}

void Ethernet_GetWorkerInfo()
{
	f_log();
	char url[64]{0};
	snprintf(url, sizeof(url), "api-get_userinfo/%s", BKanban.Cutting.Worker.UserRFID);
	SendRequest(url, true);
}

void Ethernet_GetLastCut()
{
	f_log();
	ReadCuttingParameter();
	char url[64]{0};
	snprintf(url, sizeof(url), "api-get_lastcutting/%d", BKanban.BInterface.BinterfaceId);
	SendRequest(url, false);
	BKanban.BInterface.clear();
}

void Ethernet_GetScheduleInfo(BInterface_t &binterface)
{
	f_log();
	char url[64]{0};
	snprintf(url, sizeof(url), "api-get_scheduleInfo/%d/%d/%d",
			 binterface.Schedule_Id, binterface.OriginalPo_Id, binterface.Component_Id);
	SendRequest(url, true);
}

void Ethernet_GetSize(BInterface_t &binterface)
{
	char url[64]{0};
	snprintf(url, sizeof(url), "api-get_size/%d/%d", binterface.BeamCutStartSeq_Id, binterface.BeamCutStopSeq_Id);
	SendRequest(url, true);
}

void Ethernet_StartCutting(BInterface_t &binterface)
{
	if (BKanban.Cutting.IsCutting)
	{
		RootNextion.showMessage("You are cutting");
		return;
	}
	///api-startcutting/{deviceId}/{workerId}/{scheduleId}/{clonePoId}/{seq1Id}/{seq2Id}
	char url[64]{0};
	snprintf(url, sizeof(url), "api-start_cutting/%d/%d/%d/%d/%d/%d", BKanban.Machine.id, binterface.Employee_Id,
			 binterface.BeamCutPo_Id, binterface.BeamCutStartSeq_Id, binterface.BeamCutStopSeq_Id, BKanban.BInterface.BinterfaceId);

	SendRequest(url, true);
}

void Ethernet_SubmitCuttingTime()
{
	f_log();
	//api-bmachine_cuttime/{bmachineId},{cuttime}
	char url[64]{0};
	snprintf(url, sizeof(url), "api-bmachine_cuttime/%d/%d", BKanban.Machine.id, BKanban.Cutting.SubmitCutTime);
	BKanban.Cutting.OldSubmitCutTime = BKanban.Cutting.SubmitCutTime;
	SendRequest(url, false);
}

void Ethernet_SubmitNewUser()
{
	f_log();
	// 	const char* NEW_USER_NAME = "t0";
	// const char* NEW_USER_RFID = "t1";
	// const char* USER_ID_NUMBER = "n3";
	// const char* NEW_USER_DEP = "t2";
	char name[64];
	RootNextion.GetPage_stringValue(NEW_USER_PAGE, RootNextion.NewUserHandle.NEW_USER_NAME, name, sizeof(name));
	int UserCode = RootNextion.GetPage_numberValue(NEW_USER_PAGE, RootNextion.NewUserHandle.USER_ID_NUMBER);
	String rfid = read_tagNumber();
	char dep[32];
	RootNextion.GetPage_stringValue(NEW_USER_PAGE, RootNextion.NewUserHandle.NEW_USER_DEP, dep, sizeof(dep));

	char data[256]{0};
	snprintf(data, sizeof(data), "{\"Name\":\"%s\", \"Code\":%d, \"RFID\":\"%s\", \"Dep\":\"%s\"}",
			 name, UserCode, rfid.c_str(), dep);
	RootEthernet.PostData("api-new_user", data);
	RootNextion.GotoPage(WAITING_PAGE);
}

void SendRequest(char *url, bool showWaitingPage)
{
	f_log();
	Request_t request;
	request.Update(url, showWaitingPage);
	if (RequestQueue.List.size() <= 10)
		RequestQueue.List.push(request);
}

inline void Ethernet_RequestQueue()
{
	//f_log();
	if (!RequestQueue.List.empty())
	{
		if (RootEthernet.have_old_ip)
			RootEthernet.reinitialize_ethernet_module();
		else
			RootEthernet.begin();

		if (Flag.IsRequest)
		{
			log_ln("Device is requesting");
			Serial.printf("Queue size: %d\r\n", RequestQueue.List.size());
			return;
		}

		if (!RootEthernet.client.connected())
			RootEthernet.client.connect(RootEthernet.server, RootEthernet.port);

		int i = 0;
		while (i < 10)
		{
			if (RootEthernet.client.connected())
				break;
			i++;
			delay(100);
			log_ln(i);
			RootNextion.Listening();
		}

		if (!RootEthernet.client.connected())
		{
			RequestQueue.List.front().retry++;

			if (RequestQueue.List.front().retry >= 5)
			{
				RequestQueue.List.pop();
				RootNextion.showMessage("Can not connect to server", 1000);
				EthernetInitialTimeout = 1;
			}
			return;
		}

		log("RootEthernet.client.connected() = ");
		log_ln(RootEthernet.client.connected());

		log("RootEthernet.client.status() = ");
		log_ln(RootEthernet.client.status());

		log("RootEthernet.client.getTimeout() = ");
		log_ln(RootEthernet.client.getTimeout());
		log_ln("Start connect");

		RootEthernet.SendRequest(RequestQueue.List.front().url);

		if (RequestQueue.List.front().ShowWaitingPage)
			RootNextion.Waiting(3000, "Requesting to server...");

		RequestQueue.List.pop();
		EthernetInitialTimeout = 0;
		Flag.IsRequest = true;
		Flag.RespError = false;
		Timeout=0;
	}
}
#endif // !_EthernetRequest_h
