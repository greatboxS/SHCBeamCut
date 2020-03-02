/*
 Name:		BeamCutKanban.ino
 Created:	12/18/2019 1:45:25 PM
 Author:	ryan-le-nguyen - Dell-PC
*/

int EthernetInitialTimeout = 0;
bool EthernetRenew = true;
bool get_lastcut_ok = false;

#include "Arduino.h"
#include "esp32-hal-timer.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "BeamConfig.h"
#include "Extension.h"

uint32_t tick = 0;
bool Trigger = false;
int8_t oldPage = -1;
int CircleSubmitTick = 0;
bool SubmitNow = false;
String RecentRFID = "";

void Main_Task(void *parameter);
void Nextion_Task(void *parameter);

// 1 Sec Timer
void RootRTCTimer(TimerHandle_t pxTimer)
{
	//f_log();
	CircleSubmitTick++;

	if (CircleSubmitTick >= 60)
	{
		SubmitNow = true;
		CircleSubmitTick = false;
	}

	if (Flag.IsRequest)
	{
		Timeout++;
		if (Timeout >= 5)
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

void RFID_Runing()
{
	mfrc.PCD_Init();
	if (tag_detected())
	{
		RecentRFID = read_tagNumber();
		Output_Alarm();

		switch (BKanban.CurrentPageId)
		{
		case LOGIN_PAGE:
			memccpy(BKanban.Cutting.Worker.UserRFID, RecentRFID.c_str(), 0, sizeof(BKanban.Cutting.Worker.UserRFID));
			if (BKanban.Cutting.Worker.Pass)
			{
				if (BKanban.Cutting.IsCutting)
				{
					// stop when the machine is cutting
					Ethernet_StopCutting();
				}
				else
				{
					// start
					Ethernet_StartCutting();
				}
			}
			break;

		case NEW_USER_PAGE:
			RootNextion.SetPage_stringValue(NEW_USER_PAGE, RootNextion.NewUserHandle.NEW_USER_RFID, RecentRFID.c_str());
			break;

		default:
			memccpy(BKanban.Cutting.Worker.UserRFID, RecentRFID.c_str(), 0, sizeof(BKanban.Cutting.Worker.UserRFID));
			Ethernet_GetWorkerInfo();
			break;
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

	error = xTaskCreatePinnedToCore(Main_Task, "Main_Task", 1024 * 100, NULL, 1, NULL, 1);

	Serial.println(error);

	error = xTaskCreatePinnedToCore(Nextion_Task, "Nextion_Task", 1024 * 50, NULL, 2, NULL, 0);

	Serial.println(error);

	TimerHandle_t rootTimer = xTimerCreate("RootRTCTimer", // Just a text name, not used by the kernel.
										   (1000),		   // The timer period in ticks.
										   pdTRUE,		   // The timers will auto-reload themselves when they expire.
										   NULL,		   // Assign each timer a unique id equal to its array index.
										   RootRTCTimer	// Each timer calls the same callback when it expires.
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

	for (;;)
	{
		if (!get_lastcut_ok && RequestQueue.List.empty())
		{
			Ethernet_GetLastCut();
		}

		if(BKanban.Cutting.Continue && !BKanban.Cutting.IsCutting && RequestQueue.List.empty() && !Flag.IsRequest)
		{
			ButContinueClickCallback();
		}

		RFID_Runing();

		RootEthernet.Run();

		RootNextion.Listening();

		Ethernet_RequestQueue();

		if (SubmitNow)
		{
			Ethernet_SubmitCuttingTime();
			SubmitNow = false;
		}

		//reset ethernet module every 30 min
		if (tick >= 1800)
		{
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

			if (EthernetInitialTimeout >= 10)
			{
				RootEthernet.Renew();
				EthernetInitialTimeout=0;
			}
		// 1 second trigger
		if (Trigger)
		{
			Trigger = false;
			BKanban.EthernetState = RootEthernet.Get_LANStatus();
			IPAddress ip = Ethernet.localIP();
			if (ip[0] == 0)
				EthernetStatus = false;

			if (BKanban.EthernetState == LinkON)
			{
				if (!RootEthernet.EthernetPlugIn)
				{
					//new cable plugin
					RootNextion.GotoPage(WAITING_PAGE);
					RootNextion.setNumberProperty(RootNextion.PageName[WAITING_PAGE], "tm0.tim", 60000);
					RootEthernet.begin();
					RootEthernet.EthernetPlugIn = true;

					if (BKanban.Cutting.IsCutting)
						RootNextion.GotoPage(CUTTING_PAGE);
					else
						RootNextion.GotoPage(BKanban.CurrentWindowId);
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

		// New page event
		if (BKanban.NewPageLoading)
		{
			BKanban.NewPageLoading = false;

			switch (BKanban.CurrentPageId)
			{
			case HIS_PAGE:
				BKanban.CurrentWindowId = BKanban.CurrentPageId;
				if (BKanban.CurrentPageId != oldPage)
				{
					//Ethernet_GetLastCut();
					oldPage = HIS_PAGE;
				}
				break;
			case SEARCH_PAGE:
				oldPage = -1;
				Nextion_UpdateSearchPage();
				BKanban.CurrentWindowId = BKanban.CurrentPageId;
				break;

			case INFO_PAGE:
				oldPage = -1;
				BKanban.CurrentWindowId = BKanban.CurrentPageId;
				Nextion_UpdateInfoPage();
				break;

			case CUTTING_PAGE:
				oldPage = -1;
				BKanban.CurrentWindowId = BKanban.CurrentPageId;
				Nextion_UpdateCuttingPage();
				break;

			case CONFIRM_SIZE_PAGE:
				oldPage = -1;
				break;

			case COMPONENT_PAGE:
				oldPage = -1;
				Nextion_UpdateComponentPage();
				break;

			case MACHINE_PAGE:
				oldPage = -1;
				Nextion_UpdateMachinePage();
				break;

			case NEW_USER_PAGE:
				oldPage = -1;
				break;

			case LOGIN_PAGE:
				oldPage = -1;
				Nextion_UpdateUserPage();
				break;

			default:
				break;
			}
		}
		delay(2);
	}
}

void Nextion_Task(void *parameter)
{
	for (;;)
	{
		//f_log();
		Nextion_UpdateTime();
		vTaskDelay(1000);
	}
}