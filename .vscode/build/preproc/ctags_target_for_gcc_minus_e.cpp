# 1 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
/*

 Name:		BeamCutKanban.ino

 Created:	12/18/2019 1:45:25 PM

 Author:	ryan-le-nguyen - Dell-PC

*/
# 7 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
# 8 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 2
# 9 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 2
# 10 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 2
# 11 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 2
# 12 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 2
# 13 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 2

uint32_t tick = 0;
bool Trigger = false;
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
 Serial.println(__func__);
 if (BKanban.Time.Year() == 0 && !Flag.IsRequest)
 {
  Ethernet_GetTime();
 }
}

inline void RFID_Runing()
{
 //mfrc.PCD_Init();
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

 error = xTaskCreatePinnedToCore(Main_Task, "Main_Task", 1024 * 100, 
# 134 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
                                                                    __null
# 134 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
                                                                        , 1, 
# 134 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
                                                                             __null
# 134 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
                                                                                 , 1);

 Serial.println(error);

 error = xTaskCreatePinnedToCore(Nextion_Task, "Nextion_Task", 1024 * 50, 
# 138 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
                                                                         __null
# 138 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
                                                                             , 2, 
# 138 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
                                                                                  __null
# 138 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
                                                                                      , 0);

 Serial.println(error);

 TimerHandle_t rootTimer = xTimerCreate("RootRTCTimer", // Just a text name, not used by the kernel.
             (1000), // The timer period in ticks.
             ( ( BaseType_t ) 1 ), // The timers will auto-reload themselves when they expire.
             
# 145 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
            __null
# 145 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
                , // Assign each timer a unique id equal to its array index.
             RootRTCTimer // Each timer calls the same callback when it expires.
 );

 TimerHandle_t requestTimer = xTimerCreate("RequestTimer", (5000), ( ( BaseType_t ) 1 ), 
# 149 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
                                                                          __null
# 149 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
                                                                              , RequestTimer);

 if (rootTimer == 
# 151 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
                 __null
# 151 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
                     )
 {
 }
 else
 {
  // Start the timer.  No block time is specified, and even if one was
  // it would be ignored because the scheduler has not yet been
  // started.
  if (xTimerGenericCommand( ( rootTimer ), ( ( BaseType_t ) 1 ), ( xTaskGetTickCount() ), 
# 159 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
     __null
# 159 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
     , ( 0 ) ) != ( ( ( BaseType_t ) 1 ) ))
  {
   Serial.println("Can not start root timer");
  }
  else
   Serial.println("start root timer successfully");
 }

 if (requestTimer == 
# 167 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
                    __null
# 167 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
                        )
 {
 }
 else
 {
  if (xTimerGenericCommand( ( requestTimer ), ( ( BaseType_t ) 1 ), ( xTaskGetTickCount() ), 
# 172 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
     __null
# 172 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
     , ( 0 ) ) != ( ( ( BaseType_t ) 1 ) ))
  {
   Serial.println("Can not start request timer");
  }
  else
   Serial.println("start request timer successfully");
 }
}

// the loop function runs over and over again until power down or reset
void loop()
{
 // all the things are done in tasks
 vTaskSuspend(
# 185 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino" 3 4
             __null
# 185 "e:\\Visual Code\\ESP32\\BSYS_V4\\BSYS_V4.ino"
                 );
}

void Main_Task(void *parameter)
{

 for (;;)
 {
  if (!get_last_cut_timeout && !get_lastcut_ok && RequestQueue.List.empty())
  {
   Ethernet_GetLastCut();
   last_cut_timeout_tick = 0;
   get_last_cut_timeout = true;
  }

  if (BKanban.Cutting.Continue && !BKanban.Cutting.IsCutting && RequestQueue.List.empty() && !Flag.IsRequest)
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
   EthernetInitialTimeout = 0;
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

   Serial.println(esp_get_free_heap_size());
  }

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
