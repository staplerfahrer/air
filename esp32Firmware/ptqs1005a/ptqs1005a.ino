// ============================================================
// CONFIGURATION — edit these before flashing
// ============================================================

// WiFi networks (add or remove addAP() calls in setup() to taste)
#define WIFI_SSID_1  "*** INSERT WIFI SSID HERE ***"
#define WIFI_PASS_1  "*** WIFI PASSWORD HERE ***"
#define WIFI_SSID_2  "*** EXTRA SSID ***"
#define WIFI_PASS_2  "*** PASSWORD ***"

// Server endpoint and shared passphrase (set as HTTP User-Agent)
#define SERVER_URL   "https://jacobbruinsma.com/cgi-bin/air.py/air.dat"
#define PASSPHRASE   "*** INSERT PASSPHRASE HERE ***"

// UART pins for the PTQS1005 sensor (RX, TX) — depends on your board
#define SENSOR_RX_PIN  16
#define SENSOR_TX_PIN  17

// ============================================================

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include <PTQS1005.h>
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

HardwareSerial plantowerSerial(2);
PTQS1005 ptqs(plantowerSerial);

int error_count = 0;
int post_id = 0;

void setup()
{
	Serial.begin(921600);
	Serial.println("PTQS1005 Reader");

	WiFi.mode(WIFI_STA);

	wifiMulti.addAP(WIFI_SSID_1, WIFI_PASS_1);
	wifiMulti.addAP(WIFI_SSID_2, WIFI_PASS_2);

	int n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n == 0) {
		Serial.println("no networks found");
	}

	plantowerSerial.begin(9600, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
}

void loop()
{
	ptqs.requestReadFull();
	PTQS1005::DATA data;
	int c = 0;
	while (!(ptqs.read(data)))
	{
		delay(1);
		c++;
		if (c > 1000)
		{
			error_count++;
			if (ptqs.hasError())
			{
				Serial.println("Error.");
				ptqs.reset();
			}
			else if (error_count > 10)
			{
				Serial.println("Timtout.");
				ptqs.reset();
			}
			return;
		}
	}
	error_count = 0;
	// SP = Standard Particles
	// CF=1 "Note: CF=1 should be used in the factory environment."
	// AE = Atmospheric Environment ug/m3
	/*
	https://community.purpleair.com/t/indoor-sensor-map-data-is-incorrect-due-to-use-of-cf-1-sensor-values/3021
	Note: The standard particle mass concentration value refers to the mass concentration value obtained by
	density conversion using industrial metal particles as equivalent particles, which is suitable for
	environments such as industrial production workshops. The mass concentration of particulate matter
	in the atmospheric environment is calculated by using the main pollutants in the air as equivalent
	 particles for density conversion, which is suitable for ordinary indoor and outdoor atmospheric environments.
	*/
	Serial.println("________________________________________________________________________________");
	Serial.print("PM  1.0 Atmospheric "); Serial.print(data.PM_AE_UG_1_0);  Serial.println(" ug/m3");
	bar(data.PM_AE_UG_1_0 * 4);

	Serial.print("PM  2.5 Atmospheric "); Serial.print(data.PM_AE_UG_2_5);  Serial.println(" ug/m3");
	bar(data.PM_AE_UG_2_5 * 4);

	Serial.print("PM 10.0 Atmospheric "); Serial.print(data.PM_AE_UG_10_0); Serial.println(" ug/m3");
	bar(data.PM_AE_UG_10_0 * 4);

	Serial.print("TVOC ppm   ");          Serial.println(data.TVOC_PPM);
	bar((uint16_t)round(data.TVOC_PPM * 100));

	Serial.print("HCHO mg/m3 ");          Serial.println(data.HCHO_MGM3);
	bar((uint16_t)round(data.HCHO_MGM3 * 100));

	Serial.print("CO2 ppm    ");          Serial.println(data.CO2_PPM);
	bar((uint16_t)round((data.CO2_PPM - 400) * 0.1333));

	// wait for WiFi connection
	if ((wifiMulti.run() == WL_CONNECTED))
	{
		JSONVar stats;
		stats["1_0_ae_ug"] = data.PM_AE_UG_1_0;
		stats["2_5_ae_ug"] = data.PM_AE_UG_2_5;
		stats["10_0_ae_ug"] = data.PM_AE_UG_10_0;
		stats["tvoc_ppm"] = (int)(data.TVOC_PPM*100+0.5)/100.0;
		stats["hcho_mgm3"] = (int)(data.HCHO_MGM3*100+0.5)/100.0;
		stats["co2_ppm"] = data.CO2_PPM;
		stats["humidity"] = (int)(data.HUMIDITY+0.5);
		stats["temp"] = (int)(data.TEMP+0.5);
		stats["post_id"] = post_id++;
		String json;
		json = JSON.stringify(stats);
		Serial.print(json);

		HTTPClient http;
		Serial.print("[HTTP] begin...\n");
		http.setUserAgent(PASSPHRASE);
		http.begin(SERVER_URL);

		Serial.print("[HTTP] POST...\n");
		int httpCode = http.POST(json);
		// httpCode will be negative on error
		if (httpCode > 0)
		{
			Serial.printf("[HTTP] POST... code: %d\n", httpCode);

			if (httpCode == HTTP_CODE_OK)
			{
				String payload = http.getString();
				Serial.println(payload);
			}
		}
		else
		{
			Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}

		http.end();
	}
	delay(500);
}

void bar(uint16_t x)
{
	for (uint16_t i = 0; i < x - 1; i++)
	{
		Serial.print('#');
	}
	if (x > 0) Serial.print(']');
	Serial.println();
}
