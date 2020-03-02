#ifndef _EthernetConfig_h
#define _EthernetConfig_h

#include "Ethernet.h"
#include "SPI.h"
#include <cstdint>
#include "EthernetClient.h"

// Initialize the ethernet SPI interface
// CS_PIN
typedef enum HttpMethod_t
{
	HTTP_GET,
	HTTP_POST,
	HTTP_PUT,
	HTTP_DELETE,
};

class RootEthernetClass
{
private:
	const char *GetUrl = "GET /%s HTTP/1.1";
	const char *PostUrl = "POST /%s HTTP/1.1";
	const char *PutUrl = "PUT /%s HTTP/1.1";
	const char *DeletetUrl = "DELETE /%s HTTP/1.1";

	String Url = "";
	char Host[32] = "Host: 10.4.3.41:32765";
	//const char *Connection = "Connection: close";
	const char* Connection = "Connection: Keep-Alive";
	const char *KeepAlive = "Keep-Alive: timeout=5, max=999";
	const char *ConnectionClose = "Connection: close";
    const char *ConnectionAlive = "Connection: keep-alive";
    const char *CashControl = "Cache-Control: max-age=0";
    const char *UpgradeInsecure = "Upgrade-Insecure-Requests: 1";
    const char *UserAgent = "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.122 Safari/537.36";
	//byte mac[6] = { 0x6A, 0x72, 0x19, 0x9B, 0x7F, 0xC6 };
	//byte mac[6] = { 0xF3, 0xEF, 0x90, 0xF7, 0xDD, 0x24 };
	//byte mac[6] = { 0x6C, 0xB1, 0xBD, 0x4F, 0x6C, 0xCC };
	//byte mac[6] = { 0x3A, 0xB5, 0x87, 0xC3, 0xF0, 0x27 };
	//byte mac[6] = { 0xBA, 0x98, 0xA3, 0x60, 0x60, 0xF2 };

	int InitializeConnect_Timeout = 10000;
	int tick = 0;
	bool lanConntected = false;
	bool serverConnected = false;

	uint8_t resetPin = 0;
	uint8_t CSPin = 0;
	void (*received_data_Callback)(EthernetClient &);
	void making_url(const String &_ext, uint8_t _method = HTTP_GET);

	void making_url(char *_ext, uint8_t _method);

public:
	char server[20] = "10.4.3.41";
	int port = 32765;
	bool EthernetPlugIn = false;
	
	byte mac[6] = {0xF6, 0x6C, 0x08, 0x62, 0x05, 0x06};

	EthernetClient client;

	void Run();

	void init(uint8_t _cs_pin, uint8_t _rst_pin, void (*_received_data_Callback)(EthernetClient &_stream));

	int Renew()
	{
		Reset();
		return begin();
	}

	uint8_t begin()
	{
		Ethernet.init(CSPin);

		Serial.println("Begin Ethernet configuration using DHCP");

		for (size_t i = 0; i < 4; i++)
		{
			Serial.println(Ethernet.localIP()[i], HEX);
		}

		int exception = Ethernet.begin(mac);

		if (exception == 0)
		{
			Serial.println("Failed to configure Ethernet using DHCP");
			lanConntected = false;
			// Check for Ethernet hardware present
			if (Ethernet.hardwareStatus() == EthernetNoHardware)
			{
				Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
				EthernetPlugIn = false;
			}
			if (Ethernet.linkStatus() == LinkOFF)
			{
				Serial.println("Ethernet cable is not connected.");
				EthernetPlugIn = false;
			}
		}
		else
		{
			Serial.print("  DHCP assigned IP ");
			Serial.println(Ethernet.localIP());
			lanConntected = true;
			EthernetPlugIn = true;
		}

		return exception;
	}

	bool begin_connection(uint32_t timeout = 10000);

	bool begin_Request(const String &_url, uint8_t _method = HTTP_GET);

	bool begin_Request(char *_url, uint8_t _method = HTTP_GET);

	void SendRequest(char *url);

	void SearchingPO(String _po);

	void DisconnectFromServer();

	void SendData(char *data)
	{
		client.println(data);
	}

	bool PostData(char *url, char *data)
	{
		if (!client.connected())
		{
			if (!begin_connection())
			{
				Serial.println("Connect: failed");
				return false;
			}

			Serial.println("Connect: successed");
		}

		making_url(url, HTTP_POST);
		//
		char temp[32]{0};
		int length = strlen(data);
		snprintf(temp, sizeof(temp), "Content-Length: %d", length);

		client.println(Url);
		client.println(Host);
		client.println(ConnectionAlive);
        client.println(CashControl);
        client.println(UpgradeInsecure);
        client.println(UserAgent);
		client.println("Content-Type: application/json");
		client.println(temp);
		client.println();
		client.println(data);

		Serial.println(Url);
		Serial.println(Host);
		Serial.println("Content-Type: application/json");
		Serial.printf(temp);
		Serial.println(data);
		return true;
	}

	// 1 =  connected
	// 0 = hardware not found
	// 2 = cable not connected
	uint8_t Get_LANStatus();

	void Reset()
	{
		Serial.println("Ethernet module is resetting...");
		digitalWrite(resetPin, LOW);
		delay(100);
		digitalWrite(resetPin, HIGH);
		delay(1000);
	}

	void ApplyServer()
	{
		Serial.println("ApplyServer");
		snprintf(Host, sizeof(Host), "Host: %s:%d", server, port);
		char temp[64];
		snprintf(temp, sizeof(temp), "Server: %s\r\n%s\r\n", server, Host);
		Serial.print(temp);
	}

	void SetPort(uint16_t _port)
	{
		Serial.println("Set Port");
		port = _port;
		ApplyServer();
	}

	void SetServer(const String &serverName)
	{
		Serial.println("Set server");
		memccpy(server, serverName.c_str(), 0, sizeof(server));
		ApplyServer();
	}

	void SetServer(char *serverName)
	{
		Serial.println("Set server");
		memccpy(server, serverName, 0, sizeof(server));
		ApplyServer();
	}

	void SetupHost(const String &serverName, uint16_t _port)
	{
		Serial.println("SetupHost");
		port = _port;
		memccpy(server, serverName.c_str(), 0, sizeof(server));
		ApplyServer();
		Serial.println("Setup done!");
	}

	void SetupHost(char *serverName, uint16_t _port)
	{
		Serial.println("SetupHost");
		port = _port;
		memccpy(server, serverName, 0, sizeof(server));
		ApplyServer();
		Serial.println("Setup done!");
	}

	void SetupHost()
	{
		Serial.println("SetupHost");
		ApplyServer();
		Serial.println("Setup done!");
	}
};
#endif // !EthernetConfig_h
