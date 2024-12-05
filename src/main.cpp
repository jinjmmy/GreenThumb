#include <Arduino.h>
#include <HttpClient.h>
#include <WiFi.h>
#include <inttypes.h>
#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "DHT20.h"



const int lightSensorPin = 39; 
int lightValue = 0;             
int minLight = 9999;            
int maxLight = 0;  
DHT20 DHT(&Wire1);

// This example downloads the URL "http://arduino.cc/"
char ssid[50] = "Jimmy"; // your network SSID (name)
char pass[50] = "jimmy123"; // your network password (use for WPA, or use

const int kNetworkTimeout = 30 * 1000;

const int kNetworkDelay = 1000;
void nvs_access()
{
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {

    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  // Open
  Serial.printf("\n");
  Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  }
  else
  {
    Serial.printf("Done\n");
    Serial.printf("Retrieving SSID/PASSWD\n");
    size_t ssid_len;
    size_t pass_len;
    err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
    err |= nvs_get_str(my_handle, "pass", pass, &pass_len);
    switch (err)
    {
    case ESP_OK:
      Serial.printf("Done\n");

      break;
    case ESP_ERR_NVS_NOT_FOUND:
      Serial.printf("The value is not initialized yet!\n");
      break;
    default:
      Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }

  // Close
  nvs_close(my_handle);
}

void setup()
{
  Serial.begin(9600);
  delay(1000);

  nvs_access();

  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.println(pass);
  WiFi.begin(ssid, pass);
  // WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("Type,\tStatus,\tHumidity (%),\tTemperature (C)");

  Wire1.begin(21, 22);
}

void loop()
{

  int status = DHT.read();

  String hum = String(DHT.getHumidity(), 1);
  String temp = String(DHT.getTemperature(), 1);
  lightValue = analogRead(lightSensorPin);

  delay(2000);
  int err = 0;
  WiFiClient c;
  HttpClient http(c);



  String url = "/?var=";
  url += "humidity:";
  url += hum;
  url += ",";
   url += "temperature:";
  url += temp;
  url += ",";
  url += "lightValue:";
   url += String(lightValue);

  const char *urlCharArray = url.c_str();

  // err = http.get(urlCharArray, NULL);
  err = http.get("18.118.155.86", 5000, urlCharArray, NULL);

  if (err == 0)
  {
    Serial.println("startedRequest ok");
    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();

        Serial.println(bodyLen);
        Serial.println();


        unsigned long timeoutStart = millis();
        char c;

        while ((http.connected() || http.available()) && ((millis() - timeoutStart) < kNetworkTimeout))
        {
          if (http.available())
          {
            c = http.read();

            Serial.print(c);
            bodyLen--;

            timeoutStart = millis();
          }
          else
          {
            delay(kNetworkDelay);
          }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();


  delay(2000);
}
