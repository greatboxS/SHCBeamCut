#include "EthernetConfig.h"

void RootEthernetClass::Run()
{
	if (client.available() > 0)
	{
		while (client.available() > 0)
		{
			received_data_Callback(client);
		}
	}
}

void RootEthernetClass::init(uint8_t _cs_pin, uint8_t _rst_pin, void (*_received_data_Callback)(EthernetClient &_stream))
{
	resetPin = _rst_pin;
	CSPin = _cs_pin;
	// pinMode(_rst_pin, OUTPUT);
	// digitalWrite(_rst_pin, HIGH);
	delay(1000);
	Url.reserve(128);
	received_data_Callback = _received_data_Callback;
}

void RootEthernetClass::making_url(const String &_ext, uint8_t _method)
{
	Url.remove(0, Url.length());
	switch (_method)
	{
	case HTTP_GET:
		Url = GetUrl;
		break;
	case HTTP_POST:
		Url = PostUrl;
		break;
	case HTTP_PUT:
		Url = PutUrl;
		break;
	case HTTP_DELETE:
		Url = DeletetUrl;
		break;
	default:
		Url = GetUrl;
		break;
	}

	Url.replace("%s", _ext);
}

void RootEthernetClass::making_url(char *_ext, uint8_t _method)
{
	Url.remove(0, Url.length());

	switch (_method)
	{
	case HTTP_GET:
		Url = GetUrl;
		break;
	case HTTP_POST:
		Url = PostUrl;
		break;
	case HTTP_PUT:
		Url = PutUrl;
		break;
	case HTTP_DELETE:
		Url = DeletetUrl;
		break;
	default:
		Url = GetUrl;
		break;
	}

	Url.replace("%s", _ext);
}

bool RootEthernetClass::begin_connection(uint32_t timeout)
{
	client.connect(server, port);
	char temp[64];
	snprintf(temp, sizeof(temp), "Start connect to server : %s:%d", server, port);
	Serial.println(temp);
	int count = timeout / 100;
	int i = 0;
	while (!client.connected() && i < count)
	{
		i++;
		delay(100);
		Serial.print(".");
	}
	return client.connected();
}

// false: connect falied
// true: success
bool RootEthernetClass::begin_Request(const String &_url, uint8_t _method)
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

	making_url(_url, _method);
	//

	client.println(Url);
	client.println(Host);
	client.println(ConnectionAlive);
	client.println(CashControl);
	client.println(UpgradeInsecure);
	client.println(UserAgent);
	client.println();

	Serial.println(Url);
	Serial.println(Host);

	return true;
}

// false: connect falied
// true: success
bool RootEthernetClass::begin_Request(char *_url, uint8_t _method)
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

	making_url(_url, _method);
	//

	client.println(Url);
	client.println(Host);
	client.println(ConnectionAlive);
	client.println(CashControl);
	client.println(UpgradeInsecure);
	client.println(UserAgent);
	client.println();

	Serial.println(Url);
	Serial.println(Host);

	return true;
}

void RootEthernetClass::SendRequest(char *url)
{
	making_url(url, HTTP_GET);
	//
	client.println(Url);
	client.println(Host);
	client.println(ConnectionAlive);
	client.println(CashControl);
	client.println(UpgradeInsecure);
	client.println(UserAgent);
	client.println();

	Serial.println(Url);
	Serial.println(Host);
}

void RootEthernetClass::SearchingPO(String _po)
{
	String url = "api/getpo/cart1";
	begin_Request(url);
}

void RootEthernetClass::DisconnectFromServer()
{
	client.stop();
}

uint8_t RootEthernetClass::Get_LANStatus()
{
	if (Ethernet.hardwareStatus() == EthernetNoHardware)
	{
		Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
		lanConntected = false;
		serverConnected = false;
		return 0;
	}
	else
		lanConntected = true;

	if (Ethernet.linkStatus() == LinkOFF)
	{
		Serial.println("Ethernet cable is not connected.");
		lanConntected = false;
		serverConnected = false;
		return 2;
	}
	else
		lanConntected = true;

	return 1;
}