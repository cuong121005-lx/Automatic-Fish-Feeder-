#include <WiFi.h>
#include <ESP32Servo.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h> 
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);

const char* ssid       = "Le Cuong T5";  
const char* password   = "cuong1210";     
const int feedHour     = 12; 
const long  gmtOffset_sec = 7 * 3600; 
const int   daylightOffset_sec = 0;

const int servoPin = 13;  
Servo myServo;
#define TIME_TO_SLEEP  3600        
#define uS_TO_S_FACTOR 1000000ULL  

void setup() {
  Serial.begin(115200);

  display.begin(0x3C, true); 
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE); 
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("DANG KET NOI WIFI...");
  display.println(ssid);
  display.display(); 

  Serial.printf("Dang ket noi %s ", ssid);
  WiFi.begin(ssid, password);
  
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 40) { 
      delay(500);
      Serial.print(".");
      display.print("."); 
      display.display();
      retry++;
  }

  display.clearDisplay(); 

  if(WiFi.status() == WL_CONNECTED) {
  
      configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
      struct tm timeinfo;
      
      if (getLocalTime(&timeinfo)){
        Serial.println(&timeinfo, "\nGio hien tai: %H:%M:%S");
        
        display.setTextSize(2); 
        display.setCursor(0, 0);
        display.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        display.display();
        delay(2000); 

        if (timeinfo.tm_hour == feedHour && timeinfo.tm_min < 5) {
            Serial.println("DA DEN GIO! Bat dau cho an...");
            
            display.setTextSize(1);
            display.setCursor(0, 30);
            display.println("DEN GIO AN!");
            display.println("DANG CHO AN...");
            display.display();
            
            feedFish(); 
        } else {
            Serial.printf("Chua den gio an. Hen gap lai sau 1 tieng.\n");
            
            display.setTextSize(1);
            display.setCursor(0, 30);
            display.println("CHUA DEN GIO.");
            display.println("NGU 1 TIENG...");
            display.display();
            delay(2000);
        }
      } else {
        Serial.println("Loi: Khong lay duoc gio tu mang!");
        display.setCursor(0, 0);
        display.println("LOI LAY GIO!");
        display.display();
        delay(2000);
      }
      
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
  } else {
      Serial.println("\nKhong ket noi duoc Wifi -> Bo qua kiem tra.");
      display.setCursor(0, 0);
      display.println("LOI WIFI!");
      display.display();
      delay(2000);
  }

  Serial.println("Vao che do ngu dong 60 phut...");
  
  display.clearDisplay();
  display.display(); 

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.flush(); 
  esp_deep_sleep_start();
}

void loop() {
  
}


void feedFish() {
  myServo.attach(servoPin); 
  Serial.println("Mo hu thuc an (0 -> 110)...");
  
  for (int pos = 0; pos <= 110; pos += 2) { 
    myServo.write(pos);
    delay(10); 
  }
  
  delay(500); 
  myServo.write(90); delay(200);
  myServo.write(110); delay(200);

  Serial.println("Dong hu thuc an (110 -> 0)...");
  
  for (int pos = 110; pos >= 0; pos -= 2) { 
    myServo.write(pos);
    delay(10);
  }
  
  delay(1000); 
  myServo.detach(); 
}
