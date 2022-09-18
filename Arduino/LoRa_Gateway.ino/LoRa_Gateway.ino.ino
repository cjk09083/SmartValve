//백업용 파일 20210307//

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include "images.h"

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

WiFiMulti WiFiMulti;
WiFiClient wificlient;
WiFiClient client2;

//#define ssid "cjk" //wifi id
//#define pass "09083313" 

#define ssid "mernzer1" //wifi id
#define pass "jojo5150"      //wifi pass


#define ssid2 "SK_WiFiGIGA29A8" //wifi id
#define pass2 "1910000928"      //wifi pass1

//#define ssid2 "d109-1_iptime2" //wifi id
//#define pass2 "113333555555"      //wifi pass1

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    923100000

String id = "2" ;       // 디바이스 id
float temp = 0, presure = 0, flow = 0, charge = 0, bat = 0, valve = 0;
int delay_flag = 0;
int k = 0;
int valve_step = 0;

unsigned int counter = 0;
unsigned int ctn_ms = 0;

String rssi = "RSSI --";
String packSize = "--";
String packet ;

//==================================================
//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16
SSD1306  display(0x3c, 4, 15);

////////////////////////////////////////
/////////// functions///////////////////
////////////////////////////////////////

void flag_set();
void drawFontFaceDemo();
void cbk(int packetSize);
String readSerial();
void break_str(String str);
void DisplaySetup();
void WiFisetup();
void wifi_scan();
void Http_post();

void dummy_post();


////////////////////////////////////////
/////////// setup & loop ///////////////
////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  DisplaySetup();
  wifi_scan();
  //WiFisetup();
  
  Serial.println("LoRa Receiver Callback");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  
//  if (!LoRa.begin(923E6)) {
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //LoRa.onReceive(cbk);
  LoRa.receive();
//  Serial.println(LoRa.);
  Serial.println("init ok");
}


void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) { cbk(packetSize);  }

String str;
if (Serial.available()){
  str = readSerial();   
  Serial.println(str);   
  break_str(str);
}
 
  flag_set();
}


/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


void flag_set() {
  k++;
  if (k >= 99) {
    delay_flag++;
    Serial.print(delay_flag);
    Serial.println(",");
    k = 0;
  }
  delay(10);                      // Wait
}

void drawFontFaceDemo() {
    display.clear();
    display.drawString(0, 0, packet);
    display.drawString(32, 20, rssi); // 들여쓰기, 아래로, 내용
    display.display();
}

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  Serial.println(rssi);
  Serial.println(packet);
  drawFontFaceDemo();

  Serial.println("Send OK");
  for(int i=0; i<3; i++){
    delay(100);
    LoRa.beginPacket();
    LoRa.print("Ok");
    LoRa.endPacket();
  }
  
  if(packet.startsWith("d")){
    postdata(packet);
  }
  
}

String readSerial(){   
   String str = "";   
   char ch;   
    while( Serial.available() > 0 )   
    {   
      ch = Serial.read();   
      str.concat(ch);   
      delay(10);  
    }   

    return str;     
}   

void break_str(String str){

 if (str.startsWith("a"))   
  {       
    Serial.println("motor up");   
    for(int i=0; i<3; i++){
      LoRa.beginPacket();
      LoRa.print("up");
      LoRa.endPacket();
      delay(100);
    }  
  } else if (str.startsWith("d"))  {   
    Serial.println("motor down");   
    for(int i=0; i<3; i++){
      LoRa.beginPacket();
      LoRa.print("down");
      LoRa.endPacket();    
      delay(100);
    }   
  } else if (str.startsWith("s"))  {   
    Serial.println("motor stop");  
    valve_step = 0; 
    for(int i=0; i<3; i++){
      LoRa.beginPacket();
      LoRa.print("stop");
      LoRa.endPacket();    
      delay(100);
    }   
  } else if (str.startsWith("t"))  {   
    dummy_post();
  } 
  
}

void postdata(String strdata){  
    int C_index = strdata.indexOf("c");
    int T_index = strdata.indexOf("t");
    int P_index = strdata.indexOf("p");
    int F_index = strdata.indexOf("f");
    int V_index = strdata.indexOf("v");
    int B_index = strdata.indexOf("b");
    int H_index = strdata.indexOf("h");

    int last_index = strdata.indexOf("e");

    String str_id = strdata.substring(1,C_index);
    String str_temp = strdata.substring(T_index+1,P_index);
    String str_presure = strdata.substring(P_index+1,F_index);
    String str_flow = strdata.substring(F_index+1,V_index);
    String str_valve = strdata.substring(V_index+1,B_index);
    String str_bat = strdata.substring(B_index+1,H_index);
    String str_charge = strdata.substring(H_index+1,last_index);

    Serial.print("ID :  ");
    Serial.print(str_id); 
    Serial.print(", Temp :  ");
    Serial.print(str_temp); 
    Serial.print(", Presure :  ");
    Serial.print(str_presure);
    Serial.print(", Flow :  ");
    Serial.print(str_flow);
    Serial.print(", Valve :  ");
    Serial.print(str_valve);
    Serial.print(", Bat :  ");
    Serial.print(str_bat);
    Serial.print(", Charge :  ");
    Serial.println(str_charge);
    id = str_id;
    temp = str_temp.toFloat();
    presure = str_presure.toFloat();
    flow = str_flow.toFloat();
    valve = str_valve.toFloat();
    bat = str_bat.toFloat();
    charge = str_charge.toFloat();

    //Http_post();
} 

void DisplaySetup() {
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void WiFisetup(){
  WiFi.disconnect();
  // Serial.setDebugOutput(true);
  Serial.println();
  Serial.println("[SETUP] Wifi Connecting ");
  //Serial.println(ssid,pass);
  WiFi.mode(WIFI_STA);              // Station 모드로 설정
  WiFiMulti.addAP(ssid, pass);  
  WiFiMulti.addAP(ssid2, pass2);  

  Serial.println();
  Serial.print("Waiting for WiFi... ");
  int wifi_flag = 0;
  while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print("."); 
      wifi_flag++;
      if(wifi_flag > 5){
        break;
      }
      delay(100);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(); 
}


void wifi_scan() {
  Serial.println("[SETUP] scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("[SETUP] scan done");
  if (n == 0) {
    Serial.println("[SETUP] no networks found");
  } else {
    Serial.print("[SETUP] ");
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.println(")");
      delay(10);
    }
  }
  Serial.println("");
}


void Http_post() {
  if (temp > 10000){
    Serial.print("[HTTP] Error ");

  } else {
    HTTPClient http;           //  WiFiClient client;
    http.setTimeout(3000);
    Serial.print("[HTTP] begin... Connected to ");
    Serial.println(WiFi.SSID());
    if (http.begin(client2, "http://dheco.kr/process/dhdata_insert.php")) {  // Wifi를 통한 HTTP 통신
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      Serial.print("[HTTP] POST... ");
      // start connection and send HTTP header
      String post_content = "flow=" + String(flow) + "&temp=" + String(temp) 
      + "&pressure=" + String(presure) + "&id=" + String(id) 
      + "&bat=" + String(bat) + "&charge=" + String(charge) + "&valve=" + String(valve); //전달할 데이터
      int httpCode = http.POST(post_content); //Http 통신. Code가 200이면 정상, 음수면 실패
      Serial.println(post_content);
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... success, code : %d\n", httpCode);
        String payload = http.getString();
        Serial.println(payload);  //여기에서 "to" 읽기
        try{
          int valve_index = payload.indexOf("to");
          Serial.print("valve_index : ");
          Serial.println(valve_index);
          if (valve_index > 2){
            String valve_str = payload.substring(valve_index+3);
            Serial.print("turn/");
            Serial.print(id);
            Serial.print("/");
            Serial.println(valve_str);
            for(int i=0; i<3; i++){
              LoRa.beginPacket();
              LoRa.print("turn/");
              LoRa.print(id);
              LoRa.print("/");
              LoRa.print(valve_str);
              LoRa.endPacket();
              delay(100);
            }
          }
        } catch(...){
          Serial.println("set valve error : ");
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error : %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
}


void dummy_post() {
  HTTPClient http;           //  WiFiClient client;
    http.setTimeout(3000);
    id = 2 ;
    Serial.print("[HTTP] begin... Connected to ");
    Serial.println(WiFi.SSID());
    if (http.begin(client2, "http://dheco.kr/process/dhdata_insert.php")) {  // Wifi를 통한 HTTP 통신
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      Serial.print("[HTTP] POST... ");
      // start connection and send HTTP header
      String post_content = "flow=" + String(3040) + "&temp=" + String(60) 
      + "&pressure=" + String(121) + "&id=" + String(id) 
      + "&bat=" + String(10) + "&charge=" + String(20) + "&valve=" + String(30); //전달할 데이터
      int httpCode = http.POST(post_content); //Http 통신. Code가 200이면 정상, 음수면 실패
      Serial.println(post_content);
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... success, code : %d\n", httpCode);
        String payload = http.getString();
        Serial.println(payload);
        try{
          int valve_index = payload.indexOf("to");
          Serial.print("valve_index : ");
          Serial.println(valve_index);
          if (valve_index > 2){
            String valve_str = payload.substring(valve_index+3);
            Serial.print("turn/");
            Serial.print(id);
            Serial.print("/");
            Serial.println(valve_str);
            for(int i=0; i<3; i++){
              LoRa.beginPacket();
              LoRa.print("turn/");
              LoRa.print(id);
              LoRa.print("/");
              LoRa.print(valve_str);
              LoRa.endPacket();
              delay(100);
            }
          }
          
        } catch(...){
          Serial.println("set valve error : ");
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error : %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
}
