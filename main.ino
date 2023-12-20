#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include "esp_camera.h"
#include <HTTPClient.h>
#include <DHT.h>

#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
#include "camera_pins.h"

camera_config_t config;
void config_init();

#include "myiot.h"
#include "page1.h"
#include "page2.h"
#include "page3.h"
#include "page4.h"

const int PIN_LED   = 2;
const int PIN_DH11  = 14;
const int PIN_PHOTO = 33;

DHT dht(PIN_DH11, DHT11);

using namespace std;

myIoT myIoTObj;
const myIoT defaultObj = { 0, "default\0", "12345678\0", "http://test.org\0"};

WebServer serverHTTP(80);
WiFiClient client;

void ICACHE_RAM_ATTR keyEvent(void);
volatile bool intfired=false;

String sensorServerURL;
unsigned long sensorDataInterval;
unsigned long lastSendTime = 0;
bool isSensorSendingActive = false;

String autoModeServerURL; // 자동 모드 서버 URL
String manualModeServerURL; // 수동 모드 서버 URL
unsigned long autoModeInterval = 5000; // 자동 모드 체크 간격
unsigned long lastAutoModeCheckTime = 0; // 마지막 자동 모드 체크 시간
bool isAutoMode = false; // 자동 모드 상태

String cameraServerURL;
unsigned long cameraDataInterval;
unsigned long lastCameraSendTime = 0;
bool isCameraSendingActive = false;


void handlePage1(void) {
  serverHTTP.send( 200, "text/html", page1); 
}
void handlePage2(void) {
  serverHTTP.send( 200,"text/html", page2); 
}
void handlePage3(void) {
  serverHTTP.send(200, "text/html", page3);
}
void handlePage4(void) {
  serverHTTP.send(200, "text/html", page4);
}

void getAPEvent(void) {
  DynamicJsonDocument doc(64);
  doc["ssid"] = String(myIoTObj.ssid);
  doc["pass"] = String(myIoTObj.pass);
  doc["url"] = String(myIoTObj.url);
  String buf;
  serializeJson(doc, buf);
  Serial.println(buf);
  serverHTTP.send(200, F("application/json"), buf);
}
void getSensorEvent(void) {
    StaticJsonDocument<300> doc;
    doc["url"] = sensorServerURL;
    doc["interval"] = sensorDataInterval / 1000;
    doc["state"] = isSensorSendingActive ? "on" : "off";

    doc["auto_url"] = autoModeServerURL;
    doc["led_url"] = manualModeServerURL;

    String settings;
    serializeJson(doc, settings);
    serverHTTP.send(200, "application/json", settings);
}
void getCameraEvent(void) {
  StaticJsonDocument<200> doc;
  doc["url"] = cameraServerURL;
  doc["interval"] = cameraDataInterval / 1000;
  doc["state"] = isCameraSendingActive ? "on" : "off";

  String settings;
  serializeJson(doc, settings);
  serverHTTP.send(200, "application/json", settings);  
}
bool getAutoStatus() {
  HTTPClient http;
  http.begin(autoModeServerURL);  // 올바른 URL 사용
  http.addHeader("Content-Type", "text/plain");

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);

    http.end();
    
    if (response == "off") {
      return false;
    } else {
      return true;
    }
  } else {
    Serial.print("Error on getting Auto status: ");
    Serial.println(httpResponseCode);
    http.end();
    return false;
  }
}
bool getLEDStatus() {
    HTTPClient http;
    http.begin(manualModeServerURL);
    http.addHeader("Content-Type", "text/plain");

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);

        http.end();
        
        if (response == "off") {
          return false;
        } else {
          return true;
        }
    } else {
        Serial.print("Error on getting LED status: ");
        Serial.println(httpResponseCode);
        http.end();
        return false;
    }
}

void setAPEvent(void) {
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, serverHTTP.arg(0));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;    
  }
  String ssid = doc["ssid"];
  String pass = doc["pass"];
  String url  = doc["url"];

  int ssid_len = ssid.length() + 1;
  int pass_len = pass.length() + 1;
  int url_len  = url.length() + 1;

  ssid.toCharArray(myIoTObj.ssid, ssid_len);
  pass.toCharArray(myIoTObj.pass, pass_len);
  url.toCharArray(myIoTObj.url, url_len);

  myIoTObj.tag = 0;
  writeObj(&myIoTObj);
  Serial.println(serverHTTP.arg(0));
  serverHTTP.send(200, "application/json", serverHTTP.arg(0));
}
void setSensorEvent(void) {
    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, serverHTTP.arg("plain"));

    if (error) {
      Serial.println("message: Invalid JSON");
      serverHTTP.send(400, "application/json", "{\"message\": \"Invalid JSON\"}");
      return;
    }

    sensorServerURL = doc["url"].as<String>();
    sensorDataInterval = doc["interval"].as<unsigned long>() * 1000;
    isSensorSendingActive = doc["state"] == "on";

    autoModeServerURL = doc["auto_url"].as<String>();
    manualModeServerURL = doc["led_url"].as<String>();

    Serial.println("Received sensor settings:");
    Serial.println("URL: " + sensorServerURL);
    Serial.println("Interval: " + String(sensorDataInterval));
    Serial.println("State: " + String(isSensorSendingActive ? "On" : "Off"));
    Serial.println("Received LED settings:");
    Serial.println("URL: " + autoModeServerURL);
    Serial.println("Interval: " + manualModeServerURL);

    serverHTTP.send(200, "application/json", "{\"message\": \"Settings updated\"}");
}


void setCameraEvent(void) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, serverHTTP.arg("plain"));

    if (error) {
      serverHTTP.send(400, "application/json", "{\"message\": \"Invalid JSON\"}");
      return;
    }

    cameraServerURL = doc["url"].as<String>();
    cameraDataInterval = doc["interval"].as<unsigned long>() * 1000;
    isCameraSendingActive = doc["state"] == "on";

    Serial.println("Received sensor settings:");
    Serial.println("URL: " + cameraServerURL);
    Serial.println("Interval: " + String(cameraDataInterval));
    Serial.println("State: " + String(isCameraSendingActive ? "On" : "Off"));

    serverHTTP.send(200, "application/json", "{\"message\": \"Settings updated\"}");
}

void resetEvent(void) {
  StaticJsonDocument<32> doc;
  DeserializationError error = deserializeJson(doc, serverHTTP.arg(0));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;    
  }
  ESP.restart();
}
void sendSensorData() {
  HTTPClient http;

  http.begin(sensorServerURL);
  http.addHeader("Content-Type", "application/json");

  int photo = analogRead(PIN_PHOTO);
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  String postBody = "{\"photo\": " + String(photo) + 
                    ", \"humidity\": " + String(humidity) + 
                    ", \"temp\": " + String(temp) + "}";

  Serial.println(postBody);

  int httpResponseCode = http.POST(postBody);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
void sendCameraData() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  HTTPClient http;
  WiFiClient client;

  http.begin(cameraServerURL);
  http.addHeader("accept", "application/json");
  http.addHeader("Content-Type", "image/jpeg");

  int httpResponseCode = http.POST(fb->buf, fb->len);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  esp_camera_fb_return(fb);
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);  
  dht.begin();

  config_init();
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  myIoT *obj;
  uint64_t espChipID = ESP.getEfuseMac(); //ESP.getChipId();; 
  char host[12] = "myiot-";
  char cmac[5]  = "";

  readObj(&myIoTObj);
  
  sprintf(cmac,"%4x",espChipID);
  strncat(host, cmac, 4);  
  Serial.println("---------------");

  Serial.printf("MAC Address : %6X\n\r",espChipID);
  Serial.printf("HOST Name   : %s\n\r",host);
  WiFi.mode(WIFI_STA);
  WiFi.begin(myIoTObj.ssid, myIoTObj.pass);
  
  char mode = 0;
  int cnt = 0;
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (cnt++ == 10) { 
      mode = 1; break; 
    }    
  }
  if ( mode == 0 ) {
    Serial.printf("\nHost name : %s\n\r",host);
    Serial.printf("SSID      : %s\n\r",myIoTObj.ssid);  
    Serial.printf("Connecting %s by STA Mode, IP:",host); 
    Serial.println(WiFi.localIP());
  }
  else {
    WiFi.mode(WIFI_AP);
    Serial.println("\n\rSetting soft-AP configuration ... ");
    // Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
    if ( WiFi.softAP(host) == true ) { // true == ready
      IPAddress myIP = WiFi.softAPIP();
      Serial.printf("Connecting %s by AP Mode, IP:",host);
      Serial.println(myIP);
    }
  }
  chkUserData(&myIoTObj, defaultObj);
  
  if (!MDNS.begin(host)) {
    Serial.println("Error setting up MDNS responder!");
    while(1){
      delay(1000);
    }
  }
  serverHTTP.on("/", handlePage1);
  serverHTTP.on("/next", handlePage2);
  serverHTTP.on("/page3",  handlePage3);
  serverHTTP.on("/page4",  handlePage4);

  serverHTTP.on("/getAP", getAPEvent); 
  serverHTTP.on("/getSensor", HTTP_GET, getSensorEvent);
  serverHTTP.on("/getCamera", HTTP_GET, getCameraEvent);

  serverHTTP.on("/setAP", setAPEvent); 
  serverHTTP.on("/setSensor", HTTP_POST, setSensorEvent);
  serverHTTP.on("/setCamera", HTTP_POST, setCameraEvent);

  serverHTTP.on("/reset", resetEvent);

  serverHTTP.begin();
  Serial.println("Server listing");
  delay(500);   
}

void loop(void) {
  serverHTTP.handleClient();  

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (isSensorSendingActive && millis() - lastSendTime > sensorDataInterval) {
    sendSensorData();
    lastSendTime = millis();
  }

  if (isCameraSendingActive && millis() - lastCameraSendTime > cameraDataInterval) {
    sendCameraData();
    lastCameraSendTime = millis();
  }

  // 자동 모드 체크
  if (millis() - lastAutoModeCheckTime > autoModeInterval) {
    isAutoMode = getAutoStatus();
    lastAutoModeCheckTime = millis();
  }

  // LED 상태 제어
  if (isAutoMode) {
    int photoValue = analogRead(PIN_PHOTO);
    digitalWrite(PIN_LED, photoValue < 2000);
  } else {
    digitalWrite(PIN_LED, getLEDStatus());
  }
}

void config_init() {
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
}