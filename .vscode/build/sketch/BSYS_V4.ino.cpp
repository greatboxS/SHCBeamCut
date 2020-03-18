#line 1 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
/*
 Name:		BeamCutKanban.ino
 Created:	12/18/2019 1:45:25 PM
 Author:	ryan-le-nguyen - Dell-PC
*/

#include "Arduino.h"
#include "esp32-hal-timer.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "BeamConfig.h"
#include "Extension.h"

uint32_t tick = 0;
bool Trigger = false;
int CircleSubmitTick = 0;
bool SubmitNow = false;
String RecentRFID = "";

void Main_Task(void *parameter);
void Nextion_Task(void *parameter);
void UpdateTimeTask(void *parameter);

// 1 Sec Timer
#line 25 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
void RootRTCTimer(TimerHandle_t pxTimer);
#line 95 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
void RequestTimer(TimerHandle_t pxTimer);
#line 104 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
void RFID_Runing();
#line 124 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
void setup();
#line 186 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
void loop();
#line 25 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
void RootRTCTimer(TimerHandle_t pxTimer)
{
	//f_log();
	CircleSubmitTick++;
	if (get_last_cut_timeout)
	{
		last_cut_timeout_tick++;
		if (last_cut_timeout_tick >= 5)
		{
			last_cut_timeout_tick = 0;
			get_last_cut_timeout = false;
		}
	}

	if (CircleSubmitTick >= 60)
	{
		SubmitNow = true;
		CircleSubmitTick = false;
	}

	if (Flag.IsRequest)
	{
		Timeout++;
		if (Timeout >= 3)
		{
			Flag.IsRequest = false;
			Timeout = 0;
		}
	}
	BKanban.Time.Sec()++;
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

	if (BKanban.Time.Sec() >= 60)
	{
		BKanban.Time.Sec() = 0;
		BKanban.Time.Min()++;
		if (BKanban.Time.Min() >= 60)
		{
			BKanban.Time.Min() = 0;
			BKanban.Time.Hour()++;
			if (BKanban.Time.Hour() >= 24)
			{
				BKanban.Time.Hour() = 0;
			}
		}
	}

	if (BKanban.Cutting.IsCutting)
	{
		BKanban.Cutting.sec++;
		if (BKanban.Cutting.sec >= 60)
		{
			BKanban.Cutting.sec = 0;
			BKanban.Cutting.RunTime++;
		}
		char temp[32]{0};
		snprintf(temp, sizeof(temp), "%d:%d", BKanban.Cutting.RunTime, BKanban.Cutting.sec);
		RootNextion.SetPage_stringValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.TIME, temp);
	}

	if (BKanban.Cutting.IsCutting && !Flag.IsRequest)
	{
		if (BKanban.CurrentPageId != CUTTING_PAGE && BKanban.CurrentPageId != CONFIRM_SIZE_PAGE)
			RootNextion.GotoPage(CUTTING_PAGE);
	}

	Trigger = true;
	tick++;
}

void RequestTimer(TimerHandle_t pxTimer)
{
	f_log();
	if (BKanban.Time.Year() == 0 && !Flag.IsRequest)
	{
		Ethernet_GetTime();
	}
}

inline void RFID_Runing()
{
	if (tag_detected())
	{
		RecentRFID = read_tagNumber();
		Output_Alarm();

		if (BKanban.CurrentPageId == NEW_USER_PAGE)
		{
			RootNextion.SetPage_stringValue(NEW_USER_PAGE, RootNextion.NewUserHandle.NEW_USER_RFID, RecentRFID.c_str());
		}
		else
		{
			memccpy(BKanban.Cutting.Worker.UserRFID, RecentRFID.c_str(), 0, sizeof(BKanban.Cutting.Worker.UserRFID));
			Ethernet_GetWorkerInfo();
		}
	}
}

// the setup function runs once when you press reset or power the board
void setup()
{
	SysStartup();

	Serial.begin(115200);
	while (!Serial)
		;

	BaseType_t error;

	error = xTaskCreatePinnedToCore(UpdateTimeTask, "UpdateTimeTask", 1024 * 10, NULL, 2, NULL, 0);

	Serial.println(error);

	error = xTaskCreatePinnedToCore(Main_Task, "Main_Task", 1024 * 100, NULL, 1, NULL, 1);

	Serial.println(error);

	error = xTaskCreatePinnedToCore(Nextion_Task, "Nextion_Task", 1024 * 50, NULL, 2, NULL, 0);

	Serial.println(error);

	TimerHandle_t rootTimer = xTimerCreate("RootRTCTimer", // Just a text name, not used by the kernel.
										   (1000),		   // The timer period in ticks.
										   pdTRUE,		   // The timers will auto-reload themselves when they expire.
										   NULL,		   // Assign each timer a unique id equal to its array index.
										   RootRTCTimer	   // Each timer calls the same callback when it expires.
	);

	TimerHandle_t requestTimer = xTimerCreate("RequestTimer", (5000), pdTRUE, NULL, RequestTimer);

	if (rootTimer == NULL)
	{
	}
	else
	{
		// Start the timer.  No block time is specified, and even if one was
		// it would be ignored because the scheduler has not yet been
		// started.
		if (xTimerStart(rootTimer, 0) != pdPASS)
		{
			log_ln("Can not start root timer");
		}
		else
			log_ln("start root timer successfully");
	}

	if (requestTimer == NULL)
	{
	}
	else
	{
		if (xTimerStart(requestTimer, 0) != pdPASS)
		{
			log_ln("Can not start request timer");
		}
		else
			log_ln("start request timer successfully");
	}
}

// the loop function runs over and over again until power down or reset
void loop()
{
	// all the things are done in tasks
	vTaskSuspend(NULL);
}

void Main_Task(void *parameter)
{
	bool submit_counter = false;
	for (;;)
	{
		RFID_Runing();

		RootEthernet.Run();

		Ethernet_RequestQueue();

		if (!get_last_cut_timeout && !get_lastcut_ok && RequestQueue.List.empty() && !DeviceSetting)
		{
			Ethernet_GetLastCut();
			last_cut_timeout_tick = 0;
			get_last_cut_timeout = true;
		}

		if (BKanban.Cutting.Continue && !BKanban.Cutting.IsCutting && RequestQueue.List.empty() && !Flag.IsRequest)
		{
			ButContinueClickCallback();
		}

		if (get_lastcut_ok && !submit_counter && !Flag.IsRequest)
		{
			Ethernet_SubmitCuttingTime();
			submit_counter = true;
		}

		if (SubmitNow && !DeviceSetting)
		{
			Ethernet_SubmitCuttingTime();
			SubmitNow = false;
		}

		//reset ethernet module every 30 min
		if (tick >= 1800)
		{
			mfrc.PCD_Init();
			tick = 0;
			int maintain_status = Ethernet.maintain();
			// #define DHCP_CHECK_NONE         (0)
			// #define DHCP_CHECK_RENEW_FAIL   (1)
			// #define DHCP_CHECK_RENEW_OK     (2)
			// #define DHCP_CHECK_REBIND_FAIL  (3)
			// #define DHCP_CHECK_REBIND_OK    (4)

			if (maintain_status != 2 && maintain_status != 4)
				RootEthernet.Renew();
		}

		// 1 second trigger
		if (Trigger)
		{
			Trigger = false;
			BKanban.EthernetState = RootEthernet.Get_LANStatus();
			// IPAddress ip = Ethernet.localIP();
			// if (ip[0] == 0)
			// 	EthernetStatus = false;

			if (BKanban.EthernetState == LinkON)
			{
				if (!RootEthernet.EthernetPlugIn)
				{
					//new cable plugin
					//RootNextion.GotoPage(WAITING_PAGE);
					//RootNextion.setNumberProperty(RootNextion.PageName[WAITING_PAGE], "tm0.tim", 60000);
					if (RootEthernet.have_old_ip)
						RootEthernet.reinitialize_ethernet_module();
					else
						RootEthernet.begin();

					RootEthernet.EthernetPlugIn = true;

					// if (BKanban.Cutting.IsCutting)
					// 	RootNextion.GotoPage(CUTTING_PAGE);
					// else
					// 	RootNextion.GotoPage(BKanban.CurrentWindowId);
				}
			}

			if (BKanban.EthernetState == LinkOFF)
			{
				RootEthernet.EthernetPlugIn = false;
				Serial.println("Ethernet cable is plug out");
			}

			RootNextion.setLanStatus(BKanban.CurrentWindowId, BKanban.EthernetState == 1 ? true : false);

			log_ln(esp_get_free_heap_size());
		}

		delay(2);
	}
}

void UpdateTimeTask(void *parameter)
{
	for (;;)
	{
		vTaskDelay(10);
	}
}

void Nextion_Task(void *parameter)
{
	int tick = 0;
	int old_cutTime_val = 0;
	for (;;)
	{
		RootNextion.Listening();
		
		// New page event
		if (BKanban.NewPageLoading)
		{
			BKanban.NewPageLoading = false;

			switch (BKanban.CurrentPageId)
			{
			case HIS_PAGE:
				BKanban.CurrentWindowId = BKanban.CurrentPageId;
				Nextion_UpdateHisPage();
				break;
			case SEARCH_PAGE:
				Nextion_UpdateSearchPage();
				BKanban.CurrentWindowId = BKanban.CurrentPageId;
				break;

			case INFO_PAGE:
				BKanban.CurrentWindowId = BKanban.CurrentPageId;
				Nextion_UpdateInfoPage();
				break;

			case CUTTING_PAGE:
				BKanban.CurrentWindowId = BKanban.CurrentPageId;
				Nextion_UpdateCuttingPage();
				break;

			case CONFIRM_SIZE_PAGE:
				break;

			case COMPONENT_PAGE:
				Nextion_UpdateComponentPage();
				break;

			case MACHINE_PAGE:
				Nextion_UpdateMachinePage();
				break;

			case NEW_USER_PAGE:
				break;

			case LOGIN_PAGE:
				Nextion_UpdateUserPage();
				break;

			default:
				break;
			}
		}

		if (millis() - tick > 1000 && !updateState)
		{
			tick = millis();
			Nextion_UpdateTime();
		}

		if (old_cutTime_val != BKanban.Cutting.TotalCutTimes && !updateState)
		{
			RootNextion.SetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.TOTAL_TIME, BKanban.Cutting.TotalCutTimes);
			if (BKanban.Cutting.IsCutting)
			{
				BKanban.Cutting.CurrentCutTimes++;
				RootNextion.SetPage_numberValue(CUTTING_PAGE, RootNextion.CuttingPageHandle.CUTTING_TIME, BKanban.Cutting.CurrentCutTimes);
			}
			old_cutTime_val = BKanban.Cutting.TotalCutTimes;
		}

		vTaskDelay(10);
	}
}

