
//백업용 파일 20210307//
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISnO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND  923100000 // 923.1 MHz

#define ENA     23   // GPIO21 -- Valve ENA
#define DIR     12   // GPIO12 -- Valve DIR
#define PUL     13   // GPIO13 -- Valve PUL

//#define TRIG    25   // GPIO39 -- sonic TRIG
//#define ECHO    34   // GPIO38 -- sonic ECHO

//#define PIN_SDA 21
//#define PIN_SCL 22

#define PIN_BAT 34
#define PIN_CHA 35

uint8_t T[] = {0x03, 0x04, 0x03, 0xE8, 0x00, 0x02, 0xF0, 0x59};
uint8_t P[] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB};
uint8_t F[] = {0x02, 0x04, 0x03, 0xE9, 0x00, 0x02, 0xA0, 0x48};


//uint8_t T[] = {0x03};

int data[10];

unsigned long total_step = 48 * 50 * 80; // 48바퀴 * * 80스텝?
unsigned long full_step = 48 * 5000 * 80;

int step_du = 15;           // 펄스 길이 [us]
int step_delay = 15;
        
int id = 1;
unsigned long counter = 0;
unsigned int send_flag = 0;
long mot_step = 0;

int close_dir = 1; //명령어 d가 닫기, a가 열기
int open_dir = 0;

unsigned long ctn_ms = 0;
int   mot_dir = 1, mot_ena = 0;
double valve_data = 0;
float temp = 0, pressure = 0, flow = 0, distance=0, bat = 0, charge = 0;
float temp_total = 0, pressure_total = 0, flow_total = 0, bat_total = 0, charge_total = 0;

//SSD1306 display(0x3c, 21, 22);
String rssi = "RSSI --";
String packSize = "--";
String packet ;
String mot_data = "Valve stop";
//==================================================
//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16

SSD1306  display(0x3c, 4, 15);
//HardwareSerial Serial2(2);
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display



////////////////////////////////////////
/////////// functions///////////////////
////////////////////////////////////////
void drawFontFaceDemo();
String readSerial();
String readSerial2();
void cbk(int packetSize);
void DisplaySetup();
void ValveSetup();
void break_str(String str);
float checkT();
float checkP();
float checkF();
float checkB();
float checkC();
void lcd_set();   //lcd 값 세팅 

float wire_arduino();


////////////////////////////////////////
/////////// setup & loop ///////////////
////////////////////////////////////////

void setup() {
  if(id==1){
    total_step = 1 * 50 * 80; // 48바퀴 * * 80스텝?
    full_step = 1 *5000 * 80;
    step_du = 20;
    step_delay = 50;
  }
  //DisplaySetup();
  ValveSetup();
//  pinMode(ENA, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(PUL, OUTPUT);

  Wire.begin();
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd_set();
  Serial.begin(9600);
//  Serial2.begin(9600);

  //Serial1.begin(9600,SERIAL_8N1, 22, 36); //Baud rate, parity mode, RX, TX
  Serial2.begin(9600,SERIAL_8N1, 23, 17); //Baud rate, parity mode, RX, TX

//  while (!Serial);
  Serial.println();
  Serial.println("LoRa Sender Test");
  
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(14);

  //LoRa.onReceive(cbk);
//  EEPROM.write(10, 10);
//  EEPROM.commit();

  EEPROM.begin (255);
//  EEPROM.write(1, 4);
  delay(10);
  valve_data = EEPROM.read(1);
  mot_step = valve_data * full_step / 100;
  Serial.print("Init Valve : ");
  Serial.println(valve_data);
  Serial.print(", Steps : ");
  Serial.println(mot_step);
  LoRa.receive();
  
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
  
//  if (Serial2.available()){
//    readSerial2();  
//  }
  
  if(ctn_ms++> 100){
    send_flag++;
       
    digitalWrite(DIR, mot_dir);    
    Serial.print("Valve DIR : ");
    Serial.println(mot_dir);
//    digitalWrite(ENA, HIGH);
    mot_ena = 0;  

    temp = checkT();
    temp_total += temp;
    delay(10);
    pressure = checkP();
    pressure_total += pressure;
    delay(10);
    flow = checkF();
    flow_total += flow;
    Serial.print("Temp : ");
    Serial.print(temp);
    Serial.print("(");
    Serial.print(temp_total/send_flag);

    Serial.print("), Pressure : ");
    Serial.print(pressure);
    Serial.print("(");
    Serial.print(pressure_total/send_flag);
    
    Serial.print("), Flow : ");
    Serial.print(flow);
    Serial.print("(");
    Serial.print(flow_total/send_flag);
    Serial.println(")");

    valve_data = mot_step * 100 / full_step;
    Serial.print("Valve : ");
    Serial.print(valve_data);
    EEPROM.write(1, valve_data);
    EEPROM.commit();

    wire_arduino();
    
    //bat = checkB();
    bat_total += bat;
    Serial.print(", Battery : ");
    Serial.print(bat);
    Serial.print("(");
    Serial.print(bat_total/send_flag);
    
    //charge = checkC(); 
    charge_total += charge;
    Serial.print("), Charge : ");
    Serial.print(charge);
    Serial.print("(");
    Serial.print(charge_total/send_flag);
    Serial.print(") ");
    
    lcd_set();
    
    // send packet
    if (send_flag > 3){
      lcd.init();
      lcd_set();
      temp = temp_total / send_flag;
      pressure = pressure_total / send_flag;
      flow = flow_total / send_flag;
      bat = bat_total / send_flag;
      charge = charge_total / send_flag;
      String senddata = "d"+String(id)+"c"+String(counter)+"t"+String(temp)
      +"p"+String(pressure)+"f"+String(flow)+"v"+String(valve_data)+"b"+String(bat)+"h"+String(charge)+"e";
      LoRa.beginPacket();  
      LoRa.print(senddata);
      LoRa.endPacket();
      
      Serial.print("Send : ");
      Serial.println(senddata);
      //drawFontFaceDemo();     
      temp_total = 0; pressure_total = 0; flow_total = 0; bat_total = 0; charge_total = 0;
      counter++; 
      send_flag = 0;        
    } else {  
      Serial.print("Send_flag : ");
      Serial.println(send_flag);
    }
    ctn_ms=0;
  }

  ctn_ms++;
//  Serial.println(ctn_ms);
  
  delay(1);                       

}


///////////////////////////////////// ///
/////////// functions///////////////////
////////////////////////////////////////
void break_str(String str){
  
  if (str.startsWith("a")) {       
      Serial.println("Valve close");   
      mot_data = "Valve close";
      mot_dir = close_dir;  
      mot_ena = 1;
      digitalWrite(DIR, mot_dir); 
      Serial.print("Valve Dir :");   
      Serial.println(mot_dir);   
      delay(100);
//      digitalWrite(ENA, LOW);  
      delay(10);
      for (int i = 0; i < total_step; i++){
        digitalWrite(PUL, HIGH);    
        delayMicroseconds(step_du);                     
        digitalWrite(PUL, LOW);    
        delayMicroseconds(step_delay);  
      }
      mot_step -= total_step;
      valve_data = mot_step * 100 / full_step;
      Serial.print("Valve : ");
      Serial.print(valve_data); 
      Serial.println(" Turn end");       
      EEPROM.write(1, valve_data);
      EEPROM.commit();
      delay(10);
//      digitalWrite(ENA, HIGH);  
      ctn_ms = 0;
    } else if (str.startsWith("d"))  {   
      Serial.println("Valve open");  
      mot_data = "Valve open"; 
      mot_dir = open_dir;
      mot_ena = 1;
      digitalWrite(DIR, mot_dir);  
      Serial.print("Valve Dir :");   
      Serial.println(mot_dir);    
      delay(100);
//      digitalWrite(ENA, LOW);  
      delay(10);
      for (int i = 0; i < total_step; i++){
        digitalWrite(PUL, HIGH);    
        delayMicroseconds(step_du);                     
        digitalWrite(PUL, LOW);    
        delayMicroseconds(step_delay);  
      }
      mot_step += total_step;
      valve_data = mot_step * 100 / full_step;
      Serial.print("Valve : ");
      Serial.print(valve_data); 
      Serial.println(" Turn end"); 
      EEPROM.write(1, valve_data);
      EEPROM.commit();
      delay(10);
//      digitalWrite(ENA, HIGH);  
      ctn_ms = 0;
    } else if (str.startsWith("s"))  {   
      Serial.println("Valve stop");
      mot_data = "Valve stop";   
      EEPROM.write(1, mot_step);
      EEPROM.commit();
//      digitalWrite(ENA, HIGH);
      mot_ena = 0;  
      mot_step = 0;
    } else if (str.startsWith("p"))  {   
        int diff_value = 5;
        Serial.println("Valve close");  
        mot_data = "Valve close"; 
        mot_dir = close_dir;
        mot_ena = 1;
        digitalWrite(DIR, mot_dir);  
        Serial.print("Valve Dir :");   
        Serial.println(mot_dir);   
        Serial.print("Need turns :");   
        Serial.println(diff_value);  
        delay(100);
//        digitalWrite(ENA, LOW);  
        delay(10);
        for (int i = 0; i < diff_value; i++){
         for (int j = 0; j < total_step; j++){
            digitalWrite(PUL, HIGH);    
            delayMicroseconds(step_du);                     
            digitalWrite(PUL, LOW);    
            delayMicroseconds(step_delay);  
          }
          mot_step += total_step;
          valve_data = mot_step * 100 / full_step;
          Serial.print("mot_step : ");   
          Serial.print(mot_step);
          Serial.print(", Valve1 : ");   
          Serial.println(valve_data);
        }
        Serial.println("Turn end");  
        EEPROM.write(1, valve_data);
        EEPROM.commit();
        delay(10);
//        digitalWrite(ENA, HIGH);  
        ctn_ms = 0;
  }
  
}

void drawFontFaceDemo() {
    display.clear();
    String s="ID:"+String(id)+" Send: ";
    s=s+counter;
    String sen = "T:"+String(int(temp))
    +" P:"+String(int(pressure))+" F:"+String(int(flow));
    display.drawString(0, 0, s);
    display.drawString(0, 20, mot_data);
    display.drawString(0, 40, sen);
    display.display();
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


String readSerial2(){   
   String str = "";   
   char ch;   
   int ct = 0;
   Serial.print("Recived : "); 
    while( Serial2.available() > 0 )   
    {   
      ch = Serial2.read();   
      str.concat(ch);
      Serial.print(" "); 
      Serial.print(ch,HEX); 
      data[ct] = (int) ch; 
      if(data[ct] < 0){
        data[ct] = 256 - (int)(0xFFFFFF - ch);
       }
      ct++;
      delay(10);  
    }   
    Serial.println();
    return str;     
}   


void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);

  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }

  if (packet.startsWith("u")){       
    Serial.println("Valve open");   
    mot_data = "Valve open";
    mot_dir = open_dir;  
    mot_ena = 1;
    digitalWrite(DIR, mot_dir); 
    Serial.print("Valve Dir :");   
    Serial.println(mot_dir);   
    delay(100);
//    digitalWrite(ENA, LOW);  
    delay(10);
    for (int i = 0; i < total_step; i++){
      digitalWrite(PUL, HIGH);    
      delayMicroseconds(step_du);                     
      digitalWrite(PUL, LOW);    
      delayMicroseconds(step_delay);  
    }
    mot_step -= total_step;
    valve_data = mot_step * 100 / full_step;
    Serial.print("Valve : ");
    Serial.print(valve_data);
    Serial.println("Turn end");  
    EEPROM.write(1, valve_data);
    EEPROM.commit();
    delay(10);
//    digitalWrite(ENA, HIGH);  
    ctn_ms = 0;   
  } else if (packet.startsWith("d"))  {   
    Serial.println("Valve close");  
    mot_data = "Valve close"; 
    mot_dir = close_dir;
    mot_ena = 1;
    digitalWrite(DIR, mot_dir);  
    Serial.print("Valve Dir :");   
    Serial.println(mot_dir);    
    delay(100);
//    digitalWrite(ENA, LOW);  
    delay(10);
    for (int i = 0; i < total_step; i++){
      digitalWrite(PUL, HIGH);    
      delayMicroseconds(step_du);                     
      digitalWrite(PUL, LOW);    
      delayMicroseconds(step_delay);  
    }
    mot_step += total_step;
   
    valve_data = mot_step * 100 / full_step;
    Serial.print("Valve : ");
    Serial.print(valve_data);
    Serial.println("Turn end");  
    EEPROM.write(1, valve_data);
    EEPROM.commit();
    delay(10);
//    digitalWrite(ENA, HIGH);  
    ctn_ms = 0;
  } else if (packet.startsWith("s"))  {   
    Serial.println("Valve stop");   
    mot_data = "Valve stop";
//    digitalWrite(ENA, HIGH);    
    
    valve_data = mot_step * 100 / full_step;
    Serial.print("Valve : ");
    Serial.print(valve_data);
    EEPROM.write(1, valve_data);
    EEPROM.commit();
    mot_ena = 0;
    mot_step = 0;
  } else if (packet.startsWith("t"))  {   
    int id_index = packet.indexOf("/");
    int valve_index = packet.indexOf("/",id_index+1);
    String id_str = packet.substring(id_index+1,valve_index); 
    String valve_str = packet.substring(valve_index+1); 
    int turn_id = id_str.toInt();
    int turn_valve = valve_str.toInt();
    if (turn_valve > 100){
      turn_valve = 100;
    }
    Serial.print("turn id : ");
    Serial.print(turn_id);
    Serial.print(", valve : ");
    Serial.println(turn_valve);
   
    if(turn_id == id){      
      Serial.print("It's me!!! turn valve to ");
      Serial.println(turn_valve);

      if(turn_valve > valve_data){
        int diff_value = turn_valve - valve_data;
        Serial.println("Valve close");  
        mot_data = "Valve close"; 
        mot_dir = close_dir;
        mot_ena = 1;
        digitalWrite(DIR, mot_dir);  
        Serial.print("Valve Dir :");   
        Serial.println(mot_dir);   
        Serial.print("Need turns :");   
        Serial.println(diff_value);  
        delay(100);
//        digitalWrite(ENA, LOW);  
        delay(10);
        for (int i = 0; i < diff_value; i++){
         for (int j = 0; j < total_step; j++){
            digitalWrite(PUL, HIGH);    
            delayMicroseconds(step_du);                     
            digitalWrite(PUL, LOW);    
            delayMicroseconds(step_delay);  
            mot_step += 1;
          }
          valve_data = mot_step * 100 / full_step;
          Serial.print("mot_step : ");   
          Serial.print(mot_step);
          Serial.print(", Valve1 : ");   
          Serial.println(valve_data);
        }
        Serial.println("Turn end");  
        EEPROM.write(1, valve_data);
        EEPROM.commit();
        delay(10);
//        digitalWrite(ENA, HIGH);  
        ctn_ms = 0;
      } else if (turn_valve < valve_data){
        int diff_value = valve_data - turn_valve;
        Serial.println("Valve open");   
        mot_data = "Valve open";
        mot_dir = open_dir;  
        mot_ena = 1;
        digitalWrite(DIR, mot_dir); 
        Serial.print("Valve Dir :");   
        Serial.println(mot_dir);   
        Serial.print("Need turns :");   
        Serial.println(diff_value); 
        delay(100);
//        digitalWrite(ENA, LOW);  
        delay(10);
        
        for (int i = 0; i < diff_value; i++){
         for (int j = 0; j < total_step; j++){
            digitalWrite(PUL, HIGH);    
            delayMicroseconds(step_du);                     
            digitalWrite(PUL, LOW);    
            delayMicroseconds(step_delay);  
            mot_step -= 1;
          }
          valve_data = mot_step * 100 / full_step;
          Serial.print("mot_step : ");   
          Serial.print(mot_step);
          Serial.print(", Valve1 : ");   
          Serial.println(valve_data);
        }
        Serial.println("Turn end");  
        EEPROM.write(1, valve_data);
        EEPROM.commit();
        
        delay(10);
//        digitalWrite(ENA, HIGH);  
        ctn_ms = 0;
        
      } else {
        
      Serial.println("No need turn");   
      }
      
    } 
    
  } 
  
  Serial.println("packet : "+packet);
//  display.drawString(0, 60, packet);
//  display.display();
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

void ValveSetup() {
//  pinMode(ENA, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(PUL, OUTPUT);
//  digitalWrite(ENA, HIGH);    
  digitalWrite(DIR, mot_dir);    
  digitalWrite(PUL, HIGH);    
}


float checkT(){
  Serial.print("Send T : ");
  int sizeofT = sizeof(T);
  for (int i = 0; i < sizeofT; i++) {
    Serial.print(T[i],HEX);
    Serial2.write(T[i]);
  }
  Serial.println();  
  delay(100);
  readSerial2();

  float new_data = (data[3]*256 + data[4])/pow(10,data[6]);
  return new_data;
}

float checkP(){
  Serial.print("Send P : ");
  int sizeofP = sizeof(P);
  for (int i = 0; i < sizeofP; i++) {
    Serial.print(P[i],HEX);
    Serial2.write(P[i]);
  }
  Serial.println();  
  delay(100);
  readSerial2();
  //    (int(data[3])*(16^2)+int(data[4) )/ (10^(int(data[5]*16^2)+int(data[6])))
  float new_data = (data[3]*256 + data[4])/pow(10,data[6]);
  return new_data;
}

float checkF(){
  Serial.print("Send F : ");
  int sizeofF = sizeof(F);
  for (int i = 0; i < sizeofF; i++) {
    Serial.print(F[i],HEX);
    Serial2.write(F[i]);
  }
  Serial.println();  
  delay(100);
  readSerial2();
  float new_data = (data[3]*256 + data[4])/pow(10,data[6]);
  return new_data;
}

float checkB(){
  int analog_value=analogRead(PIN_BAT);
  long volt=analog_value*(5*3.47*2);
  float volt2=(volt/4095.0);

  float B_data=(1-((4-(volt2-16))/4))*100;
//  Serial.print("B(");
//  Serial.print(volt2);
//  Serial.print("/");
//  Serial.print(B_data);
//  Serial.print(")");
  if (B_data <0){
    B_data = 0.1;  
  } else if (B_data > 100){
    B_data = 99.9;  
  }
  return B_data;
}

float checkC(){
  int sensorValue = analogRead(PIN_CHA);  
  long Volt = sensorValue*(5*3.3*2);
  float c_value = (Volt/4095.0);

 
  return c_value;
}


float wire_arduino(){
  int a[10];
  int i=0, sum=0, sum2=0;
  Wire.requestFrom(1,9);
  while(Wire.available()>0){
    a[i] = (int) Wire.read();
    Serial.print(a[i]);
    Serial.print(" ");
    if(a[i] != 255){
      if(i<4){
        sum = int(a[i])*(pow(10,4-i-1))+sum;
      } else if (i > 4){
        sum2 = int(a[i])*(pow(10,8-i))+sum2;
      }
    }
    i++;
  }
  Serial.println();
  Serial.print(sum);
  Serial.print(',');
  Serial.print(sum2);
  Serial.println();
    
  long Volt = sum*4*(5*3.3*2);
//  float c_value = (Volt/4095.0);
  float c_value = sum/10;

  long volt=sum2*4*(5*3.47*2);
  float volt2=(volt/4095.0);

//  float B_data=(1-((4-(volt2-16))/4))*100;
  float B_data=sum2/10;

//  Serial.print("B(");
//  Serial.print(volt2);
//  Serial.print("/");
//  Serial.print(B_data);
//  Serial.print(")");
  if (B_data <0){
    B_data = 0.1;  
  } else if (B_data > 100){
    B_data = 99.9;  
  }
//    Serial.println();
//    Serial.print(c_value);
//    Serial.print(',');
//    Serial.println(B_data);
  if((c_value>0)&&(c_value<2000)){
    charge = c_value;
  }
  if((B_data>0)&&(B_data<2000)){
    bat = B_data;
  }
    
  return c_value, B_data;
}

void lcd_set(){
//  lcd.init(); 
  temp = temp_total / send_flag;
  pressure = pressure_total / send_flag;
  flow = flow_total / send_flag;
  bat = bat_total / send_flag;
  charge = charge_total / send_flag;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("B:");
  lcd.print(bat);
  lcd.print(", C:");
  lcd.print(charge);
  
  lcd.setCursor(0,1);
  lcd.print("S:");
  lcd.print(temp);
  lcd.print("/");
  lcd.print((int)pressure);
  lcd.print("/");
  lcd.print((int)flow);
//  lcd.print("         ");
}
