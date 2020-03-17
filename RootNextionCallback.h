#ifndef _RootNextionCallback_h
#define _RootNextionCallback_h
// page his
void ButContinueClickCallback();
void ButGoSearchClickCallBack();
void ButSettingMachineClickCallback();
void ButSettingUserClickCallback();
// page search
void ButSearchingClickCallback();
void ButSelectScheduleCallback(uint8_t componentId);
void ButSearchGobackClickCallback();
// page info
void ButInfoGoNextClickCallback();
void ButInfoGoBackClickCallback();
// page cutting
void ButStartCuttingClickCallback();
void ButStopCuttingClickCallback();
void ButGetSizeClickCallback();
void ButSelectSizeClickCallback(uint8_t componentId);
void ButCuttingBackClickCallback();
// page confirm
void ButConfirmFinishSize();
void ButConfirmNotYetFinishSize();
//page component
void ButSelectComponentClickCallback(uint8_t componentId);
void ButGobackClickCallback();
//page machine
void ButSaveMachineClickCallback();
void ButSaveServerClickCallback();
void ButResetClickCallback();
void ButSaveMacClickCallback();

// new user page
void ButAddNewUserClickCallback();
//page login
void ButModifyClickCallback();
// message and waiting page
void WaitingTimeOutCallback();

void Nextion_UpdateTime();
void Nextion_UpdateFooter();
void Nextion_UpdateHisPage();
void Nextion_UpdateSearchPage();
void Nextion_UpdateInfoPage();
void Nextion_UpdateCuttingPage();
void Nextion_UpdateComponentPage();
void Nextion_UpdateUserPage();
void Nextion_UpdateMachinePage();

const char *txtTime[] = {"r3.val", "r4.val", "r5.val", "r0.val", "r1.val", "r2.val"};
bool updateState = false;
//printh 65 00 C8 00 FF FF FF

//-------------------------------His page [page 0] callback functions ----------------------------
void ButContinueClickCallback()
{
	f_log();
	BKanban.Cutting.Continue = true;
	if (!BKanban.BInterface.Finish)
	{
		BKanban.Cutting.clear();
		Ethernet_GetComponent();
		//Ethernet_GetScheduleInfo(BKanban.BInterface);
	}
}

void ButGoSearchClickCallBack()
{
	f_log();
	RootNextion.GotoPage(SEARCH_PAGE);
	BKanban.CurrentWindowId = SEARCH_PAGE;
}

void ButSettingMachineClickCallback()
{
	f_log();
	RootNextion.SetPage_stringValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.MACHINE_NAME, BKanban.Machine.MachineName);
	RootNextion.SetPage_stringValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.MACHINE_CODE, BKanban.Machine.MachineCode);
	RootNextion.SetPage_stringValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.SERVER_IP, RootEthernet.server);
	RootNextion.SetPage_numberValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.SERVER_PORT, RootEthernet.port);

	char *props[6] = {"n1", "n2", "n3", "n4", "n5", "n6"};
	for (int i = 0; i < 6; i++)
	{
		RootNextion.SetPage_numberValue(MACHINE_PAGE, props[i], RootEthernet.mac[i]);
	}

	RootNextion.GotoPage(MACHINE_PAGE);
	BKanban.CurrentWindowId = MACHINE_PAGE;
}

void ButSettingUserClickCallback()
{
	f_log();
	RootNextion.GotoPage(NEW_USER_PAGE);
	BKanban.CurrentWindowId = NEW_USER_PAGE;
}

//-------------------------------search page [page 1] callback functions --------------------------
void ButSearchingClickCallback()
{
	f_log();
	BKanban.Cutting.clear();
	Ethernet_GetSchedule();
}

void ButSelectScheduleCallback(uint8_t componentId)
{
	f_log();
	BKanban.SelectedSchedule = BKanban.Cutting.SelectCurrentSchedule(componentId - RootNextion.SearchPageHandle.FIRST_SELECT_PO_BUT_ID);

	Ethernet_GetComponent();
}

void ButSearchGobackClickCallback()
{
	RootNextion.GotoPage(HIS_PAGE);
	//Ethernet_GetLastCut();
}

void ButInfoGoNextClickCallback()
{
	f_log();
	RootNextion.GotoPage(CUTTING_PAGE);
	BKanban.CurrentWindowId = CUTTING_PAGE;
	//RootNextion.SetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.START_SEQ, 1);
	RootNextion.SetPage_numberValue(CUTTING_PAGE,
									RootNextion.CuttingPageHandle.STOP_SEQ, BKanban.Cutting.ScheduleInfo.SeqList.size());
}

void ButInfoGoBackClickCallback()
{
	f_log();
	RootNextion.GotoPage(SEARCH_PAGE);
	BKanban.CurrentWindowId = SEARCH_PAGE;
}

//-------------------------------cutting page [page 2] callback functions --------------------------
void ButStartCuttingClickCallback()
{
	f_log();
	//start cutting
	Ethernet_GetSize();
	Ethernet_StartCutting();
}

void ButStopCuttingClickCallback()
{
	f_log();
	Ethernet_StopCutting();
	// stop cutting
}

void ButGetSizeClickCallback()
{
	f_log();
	//
	if(BKanban.Cutting.IsCutting)
	{
		RootNextion.showMessage("Please finish your cut first");
		return;
	}
	Ethernet_GetSize();
}

void ButSelectSizeClickCallback(uint8_t componentId)
{
	f_log();
	if (!BKanban.Cutting.IsCutting)
		return;
	BKanban.SelectedSize = BKanban.Cutting.SelectCurrentSize(componentId - 40);
	RootNextion.SetPage_numberValue(CONFIRM_SIZE_PAGE, RootNextion.ConfirmSizeHandle.CUT_QTY, BKanban.SelectedSize->CuttingQty);
	RootNextion.SetPage_numberValue(CONFIRM_SIZE_PAGE, RootNextion.ConfirmSizeHandle.SIZE_QTY, BKanban.SelectedSize->SizeQty);
	RootNextion.GotoPage(CONFIRM_SIZE_PAGE);
}

void ButCuttingBackClickCallback()
{
	f_log();
	if (BKanban.Cutting.IsCutting)
		RootNextion.showMessage("You are cutting. Finish cutting first!");
	else
	{
		RootNextion.GotoPage(INFO_PAGE);
		Ethernet_GetScheduleInfo();
	}
}

void ButConfirmFinishSize()
{
	f_log();
	Ethernet_ConfirmSize(BKanban.SelectedSize->SizeQty);
}

void ButConfirmNotYetFinishSize()
{
	f_log();
	int sizeqty = RootNextion.GetPage_numberValue(CONFIRM_SIZE_PAGE, RootNextion.ConfirmSizeHandle.TOTAL_QTY);
	Ethernet_ConfirmSize(sizeqty);
}

//-------------------------------component page [page 4] callback functions --------------------------
void ButSelectComponentClickCallback(uint8_t componentId)
{
	f_log();
	BKanban.SelectedComponent = BKanban.Cutting.SelectCurrentComponent(componentId - RootNextion.ComponentHandle.FIST_SELECT_COMPONENT_ID);
	Ethernet_GetScheduleInfo();
}

void ButGobackClickCallback()
{
	f_log();
	RootNextion.GotoPage(SEARCH_PAGE);
}

//-------------------------------machine page [page 6] callback functions --------------------------
void ButSaveMachineClickCallback()
{
	f_log();
	RootNextion.GetPage_stringValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.MACHINE_CODE,
									BKanban.Machine.MachineCode, sizeof(BKanban.Machine.MachineCode));
	RootNextion.GetPage_stringValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.MACHINE_NAME,
									BKanban.Machine.MachineName, sizeof(BKanban.Machine.MachineName));

	if (strlen(BKanban.Machine.MachineCode) == 0)
		BKanban.Machine.MachineCode[0] = 'A';
	if (strlen(BKanban.Machine.MachineName) == 0)
		BKanban.Machine.MachineName[0] = 'A';

	log("DeviceName: ");
	log_ln(BKanban.Machine.MachineName);
	log("SystemCode: ");
	log_ln(BKanban.Machine.MachineCode);

	char url[64]{0};
	snprintf(url, sizeof(url), "api-get_bdevice/%s/%s", BKanban.Machine.MachineCode, BKanban.Machine.MachineName);
	SendRequest(url, true);
}

void ButSaveServerClickCallback()
{
	f_log();
	RootNextion.GetPage_stringValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.SERVER_IP, RootEthernet.server,
									sizeof(RootEthernet.server));
	RootEthernet.port = RootNextion.GetPage_numberValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.SERVER_PORT);
	RootEthernet.SetupHost();
	Serial.println(RootEthernet.port);
	lcal::SaveServerInformation(RootEthernet.server, sizeof(RootEthernet.server), RootEthernet.port);
}

void ButResetClickCallback()
{
	f_log();
	esp_restart();
}

void ButSaveMacClickCallback()
{
	f_log();
	char *props[6] = {"n1", "n2", "n3", "n4", "n5", "n6"};
	byte mac[6]{0};
	for (int i = 0; i < 6; i++)
	{
		uint32_t mac_element = RootNextion.GetPage_numberValue(MACHINE_PAGE, props[i]);
		Serial.printf("mac[%d]: %d\r\n", i, mac_element);
		if (mac_element > 255 || mac_element == 0 || mac_element == -1)
		{
			RootNextion.showMessage("Invalid mac number");
			return;
		}
		mac[i] = (byte)mac_element;
	}

	memccpy(RootEthernet.mac, mac, 0, sizeof(mac));
	EEPROM_SaveMac(false, RootEthernet.mac);
	RootNextion.showMessage("Saved Successfully");
}

//-------------------------------new user page [page 9] callback functions --------------------------
void ButAddNewUserClickCallback()
{
	f_log();
	Ethernet_SubmitNewUser();
}

//-------------------------------login page [page 10] callback functions --------------------------
void ButModifyClickCallback()
{
	f_log();
}

//-------------------------------Time out page -------------------------------------------------

void WaitingTimeOutCallback()
{
	f_log();
	Flag.IsRequest = false;

	if (BKanban.CurrentPageId == COMPONENT_PAGE)
		RootNextion.GotoPage(INFO_PAGE);
	else
		RootNextion.GotoPage(BKanban.CurrentWindowId);
}

void Nextion_UpdateTime()
{
	while (updateState)
	{
		yield();
	}
	for (int i = 0; i < 6; i++)
	{
		char temp[64]{0};
		snprintf(temp, sizeof(temp), "%s.%s", RootNextion.PageName[BKanban.CurrentPageId], txtTime[i]);
		RootNextion.setNumberProperty(temp, BKanban.Time.arr[i]);
	}

	// RootNextion.setNumberProperty("r3.val", BKanban.Time.arr[0]);

	// RootNextion.setNumberProperty("r4.val", BKanban.Time.arr[1]);

	// RootNextion.setNumberProperty("r5.val", BKanban.Time.arr[2]);

	// RootNextion.setNumberProperty("r0.val", BKanban.Time.arr[3]);

	// RootNextion.setNumberProperty("r1.val", BKanban.Time.arr[4]);

	// RootNextion.setNumberProperty("r2.val", BKanban.Time.arr[5]);
}

//-----------------------------------------------------------------------------------
void Nextion_UpdateFooter()
{
	f_log();
}
void Nextion_UpdateHisPage()
{
	f_log();
	updateState = true;
	RootNextion.SetPage_stringValue(HIS_PAGE, RootNextion.HisPageHandle.PO_NUMBER, BKanban.BInterface.PoNumber.c_str());
	RootNextion.SetPage_stringValue(HIS_PAGE, RootNextion.HisPageHandle.COMPONENT, BKanban.BInterface.Component.c_str());
	RootNextion.SetPage_stringValue(HIS_PAGE, RootNextion.HisPageHandle.START_TIME, BKanban.BInterface.StartTime.c_str());
	RootNextion.SetPage_stringValue(HIS_PAGE, RootNextion.HisPageHandle.STOP_TIME, BKanban.BInterface.StopTime.c_str());

	RootNextion.setPage_stringAsNumberProperty(HIS_PAGE, RootNextion.HisPageHandle.PO_QTY, BKanban.BInterface.PoQty);
	RootNextion.setPage_stringAsNumberProperty(HIS_PAGE, RootNextion.HisPageHandle.CUTTING_QTY, BKanban.BInterface.CutQty);
	RootNextion.setPage_stringAsNumberProperty(HIS_PAGE, RootNextion.HisPageHandle.CUTTING_TIME, BKanban.BInterface.CutTime);
	RootNextion.setPage_stringAsNumberProperty(HIS_PAGE, RootNextion.HisPageHandle.START_SEQ, BKanban.BInterface.StartSeq);
	RootNextion.setPage_stringAsNumberProperty(HIS_PAGE, RootNextion.HisPageHandle.STOP_SEQ, BKanban.BInterface.StopSeq);
	updateState = false;
}

void Nextion_UpdateSearchPage()
{
	f_log();
	updateState = true;
	for (uint8_t i = 0; i < BKanban.Cutting.ScheduleList.size(); i++)
	{
		RootNextion.SetPage_stringValue(SEARCH_PAGE, RootNextion.NexPo(i), BKanban.Cutting.ScheduleList.at(i).PoNumber);
		RootNextion.setPage_stringAsNumberProperty(SEARCH_PAGE, RootNextion.NexText(i), BKanban.Cutting.ScheduleList.at(i).Quantity);

		if (BKanban.Cutting.ScheduleList.at(i).Cutting)
			RootNextion.SetPage_propertyForceColor(SEARCH_PAGE, RootNextion.NexPo(i), NEX_COL_RED);
		else
			RootNextion.SetPage_propertyForceColor(SEARCH_PAGE, RootNextion.NexPo(i), 0);
	}
	updateState = false;
}
void Nextion_UpdateInfoPage()
{
	f_log();
	updateState = true;
	RootNextion.SetPage_stringValue(INFO_PAGE, RootNextion.InfoPageHandle.PO_NUMBER, BKanban.Cutting.ScheduleInfo.PoNumber);
	RootNextion.SetPage_stringValue(INFO_PAGE, RootNextion.InfoPageHandle.MODEL_NUMBER, BKanban.Cutting.ScheduleInfo.Model);
	RootNextion.SetPage_stringValue(INFO_PAGE, RootNextion.InfoPageHandle.MODEL_NAME, BKanban.Cutting.ScheduleInfo.ModelName);
	RootNextion.SetPage_stringValue(INFO_PAGE, RootNextion.InfoPageHandle.ARTICLE, BKanban.Cutting.ScheduleInfo.Article);
	RootNextion.SetPage_stringValue(INFO_PAGE, RootNextion.InfoPageHandle.PO_LINE, BKanban.Cutting.ScheduleInfo.Line);
	RootNextion.SetPage_numberValue(INFO_PAGE, RootNextion.InfoPageHandle.PO_QTY, BKanban.Cutting.ScheduleInfo.Quantity);

	log("SeqList.size(): ");
	log_ln(BKanban.Cutting.ScheduleInfo.SeqList.size());

	for (uint8_t i = 0; i < BKanban.Cutting.ScheduleInfo.SeqList.size(); i++)
	{
		uint8_t seqIndex = BKanban.Cutting.ScheduleInfo.SeqList.at(i).SeqNo - 1;
		uint16_t seqQty = BKanban.Cutting.ScheduleInfo.SeqList.at(i).SeqQty;

		RootNextion.setPage_stringAsNumberProperty(INFO_PAGE, RootNextion.NexText(seqIndex), seqQty);

		log_ln(BKanban.Cutting.ScheduleInfo.SeqList.at(i).Finish);
		if (BKanban.Cutting.ScheduleInfo.SeqList.at(i).Finish)
			RootNextion.SetPage_propertyForceColor(INFO_PAGE, RootNextion.NexText(seqIndex), NEX_COL_RED);
		else
			RootNextion.SetPage_propertyForceColor(INFO_PAGE, RootNextion.NexText(seqIndex), 31);
	}
	updateState = false;
}
void Nextion_UpdateCuttingPage()
{
	f_log();
	updateState = true;
	RootNextion.SetPage_stringValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.PO_NUMBER, BKanban.Cutting.ScheduleInfo.PoNumber);
	RootNextion.setPage_stringAsNumberProperty(CUTTING_PAGE, RootNextion.CuttingPageHandle.TOTAL_SEQ, BKanban.Cutting.ScheduleInfo.SeqList.size());
	RootNextion.SetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.SEQ_QTY, BKanban.Cutting.TotalSelectedQty);

	if (BKanban.SelectedComponent == NULL)
		RootNextion.SetPage_stringValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.COMPONENT, BKanban.BInterface.Component.c_str());
	else
		RootNextion.SetPage_stringValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.COMPONENT, BKanban.SelectedComponent->Name);

	RootNextion.SetPage_stringValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.USER_NAME, BKanban.Cutting.Worker.UserName);
	RootNextion.SetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.CUTTING_TIME, BKanban.Cutting.CurrentCutTimes);
	RootNextion.SetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.TOTAL_TIME, BKanban.Cutting.TotalCutTimes);
	RootNextion.SetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.CUTTING_QTY, BKanban.Cutting.TotalCuttingQty);

	char temp[32]{0};
	snprintf(temp, sizeof(temp), "%d:%d", BKanban.Cutting.RunTime, BKanban.Cutting.sec);
	RootNextion.SetPage_stringValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.TIME, temp);
	for (uint8_t i = 0; i < BKanban.Cutting.SizeList.size(); i++)
	{
		uint8_t sizeid = BKanban.Cutting.SizeList[i].SizeId;
		RootNextion.setPage_stringAsNumberProperty(CUTTING_PAGE, RootNextion.NexSize(sizeid), BKanban.Cutting.SizeList[i].SizeQty);
		RootNextion.setPage_stringAsNumberProperty(CUTTING_PAGE, RootNextion.NexText(sizeid), BKanban.Cutting.SizeList[i].CuttingQty);

		if (BKanban.Cutting.SizeList.at(i).Finish)
		{
			RootNextion.SetPage_propertyForceColor(CUTTING_PAGE, RootNextion.NexText(sizeid), NEX_COL_RED);
		}
		else
		{
			RootNextion.SetPage_propertyForceColor(CUTTING_PAGE, RootNextion.NexText(sizeid), 1152);
		}
	}

	updateState = false;
}
void Nextion_UpdateComponentPage()
{
	f_log();
	updateState = true;
	for (uint8_t i = 0; i < BKanban.Cutting.ComponentList.size(); i++)
	{
		log_ln(BKanban.Cutting.ComponentList.at(i).Name);
		RootNextion.SetPage_stringValue(COMPONENT_PAGE, RootNextion.NexText(i), BKanban.Cutting.ComponentList.at(i).Name);
	}
	updateState = false;
}
void Nextion_UpdateUserPage()
{
	f_log();
	RootNextion.SetPage_stringValue(LOGIN_PAGE, RootNextion.LoginPageHandle.NEW_USER_NAME, BKanban.Cutting.Worker.UserName);
	RootNextion.SetPage_stringValue(LOGIN_PAGE, RootNextion.LoginPageHandle.NEW_USER_RFID, BKanban.Cutting.Worker.UserRFID);
	RootNextion.SetPage_numberValue(LOGIN_PAGE, RootNextion.LoginPageHandle.USER_ID_NUMBER, BKanban.Cutting.Worker.UserCode);
	RootNextion.SetPage_stringValue(LOGIN_PAGE, RootNextion.LoginPageHandle.JOB_TITLE, BKanban.Cutting.Worker.JobTitle.c_str());
	RootNextion.SetPage_stringValue(LOGIN_PAGE, RootNextion.LoginPageHandle.NEW_USER_DEP, BKanban.Cutting.Worker.Department.c_str());
}
void Nextion_UpdateMachinePage()
{
	f_log();
	//RootNextion.SetPage_stringValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.MACHINE_NAME, BKanban.Machine.MachineName);
	//RootNextion.SetPage_stringValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.MACHINE_CODE, BKanban.Machine.MachineCode);

	char localIp[32]{0};
	snprintf(localIp, sizeof(localIp), "%d.%d.%d.%d", Ethernet.localIP()[0], Ethernet.localIP()[1],
			 Ethernet.localIP()[2], Ethernet.localIP()[3]);

	RootNextion.SetPage_stringValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.LOCAL_IP, localIp);
	//RootNextion.SetPage_stringValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.SERVER_IP, RootEthernet.server);
	//RootNextion.SetPage_numberValue(MACHINE_PAGE, RootNextion.SettingMachinePageHandle.SERVER_PORT, RootEthernet.port);
}
//------------------------------------------------------------------------------------------------
#endif // !_RootNextionCallback_h
