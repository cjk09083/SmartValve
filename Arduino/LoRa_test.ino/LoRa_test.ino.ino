//백업용 파일 20210307//

/*
  1. 일반(standby) 모드: 1
  LoRa.idle();

  2. 수신 전용 모드 : 2
  LoRa.receive();

  3. 슬립 모드 :  3
  LoRa.sleep();

  4. txpower 변경 :
  LoRa.setTxPower(txPower);  // TX power in dB, defaults to 17, 14dB=25mW

  5. 주파수 변경 :
  LoRa.setFrequency(frequency); // 920.9(q) 923.1(w) 923.3(e)

  6. 점유주파수대역폭 변경:
  LoRa.setSignalBandwidth(signalBandwidth);  // 62.5E3(a), 125E3(d)
  // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3(default), 250E3, and 500E3

  7. Invert IQ Signals :
  LoRa.enableInvertIQ(); // z
  LoRa.disableInvertIQ(); // c

  8. 데이터 패킷 전송: t
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  9. Lora 통신 시작 중지:
  LoRa.begin(frequency); // x
  LoRa.end(); // s


  10.Coding Rate 변경 :
  LoRa.setCodingRate4(5); // Supported values are between 5 and 8. defaults is 5

  11. Preamble Length 변경 :
  LoRa.setPreambleLength(8); // Supported values are between 6 and 65535. defaults is 8

*/

#include <SPI.h>
#include <LoRa.h>

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    923100000
#define LOW_BAND    920900000
#define HIGH_BAND    923300000

#define Bandwidth1    62.5E3
#define Bandwidth2    125E3
#define Bandwidth3    7.8E3

String test_data = "test" ;       // 디바이스 id
int flag = 1;
long sendtime = 0;
int mode_set = 1;
int mod_flag = 0;

int send_delay = 19999;

String readSerial();
void break_str(String str);

////////////////////////////////////////
/////////// setup & loop ///////////////
////////////////////////////////////////

void setup() {
  Serial.begin(9600);

  LoRa.setPins(SS, RST, DI0);
  //  if (!LoRa.begin(923E6)) {
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(20);
  //  LoRa.setCodingRate4(1);
  //  LoRa.setPreambleLength(8);
  LoRa.setSignalBandwidth(Bandwidth3);
  Serial.println("init ok");
}


void loop() {

  if (Serial.available()) {
    String str = readSerial();
    Serial.println(str);
    break_str(str);
  }
  if (mode_set == 1) {
    if ((millis() - sendtime) > send_delay) {
      String send_str = test_data + flag;
      
      Serial.println(send_str);
      LoRa.beginPacket();
//      LoRa.print(send_str);
      LoRa.endPacket();
      sendtime = millis();
      flag++;
      if (flag > 100) {
        flag = 0;
      }
    } else {
      if ( ((millis() - sendtime) % 1000) == 0) {
        if (((millis() - sendtime) / 1000) != mod_flag) {
          Serial.println((millis() - sendtime) / 1000);
          mod_flag = (millis() - sendtime) / 1000;
        }
        //        Serial.println((millis() - sendtime)%1000);
      }
    }
  }
}


/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


String readSerial() {
  String str = "";
  char ch;
  while ( Serial.available() > 0 )
  {
    ch = Serial.read();
    str.concat(ch);
    delay(10);
  }

  return str;
}

void break_str(String str) {
  if (str.startsWith("1")) {
    Serial.println("set Standby mode ");
    mode_set = 1;
    LoRa.idle();
  } else if (str.startsWith("2"))  {
    Serial.println("set receive mode ");
    mode_set = 2;
    LoRa.receive();
  } else if (str.startsWith("3"))  {
    Serial.println("set sleep mode ");
    mode_set = 3;
    LoRa.sleep();

  } else if (str.startsWith("q"))  {
    Serial.print("setFrequency to ");
    Serial.println(LOW_BAND);
    LoRa.setFrequency(LOW_BAND);
  } else if (str.startsWith("w"))  {
    Serial.print("setFrequency to ");
    Serial.println(921500000);
    LoRa.setFrequency(921500000);
  } else if (str.startsWith("e"))  {
    Serial.print("setFrequency to ");
    Serial.println(921900000);
    LoRa.setFrequency(921900000);
  } else if (str.startsWith("i"))  {
    Serial.print("setFrequency to ");
    Serial.println(922100000);
    LoRa.setFrequency(922100000);
  } else if (str.startsWith("o"))  {
    Serial.print("setFrequency to ");
    Serial.println(922700000);
    LoRa.setFrequency(922700000);
  } else if (str.startsWith("p"))  {
    Serial.print("setFrequency to ");
    Serial.println(923300000);
    LoRa.setFrequency(923300000);
  } else if (str.startsWith("u"))  {
    Serial.print("setFrequency to ");
    Serial.println(BAND);
    LoRa.setFrequency(BAND);
    //    /*
    //      } else if (str.startsWith("z"))  {
    //        Serial.println("enableInvertIQ");
    //        LoRa.enableInvertIQ();
    //      } else if (str.startsWith("c"))  {
    //        Serial.println("disableInvertIQ");
    //        LoRa.disableInvertIQ();
    //    */
  } else if (str.startsWith("j"))  {
    Serial.println("set delay 40s");
    send_delay = 39999;
  } else if (str.startsWith("k"))  {
    Serial.println("set delay 20s");
    send_delay = 19999;
  } else if (str.startsWith("l"))  {
    Serial.println("set delay 1ms");
    send_delay = 0;
  } else if (str.startsWith("a"))  {
    Serial.print("setSignalBandwidth to ");
    Serial.println(Bandwidth3);
    LoRa.setSignalBandwidth(Bandwidth3);
  } else if (str.startsWith("d"))  {
    Serial.print("setSignalBandwidth to ");
    Serial.println(Bandwidth2);
    LoRa.setSignalBandwidth(Bandwidth2);

  } else if (str.startsWith("s"))  {
    Serial.println("Lora end");
    mode_set = 4;
    LoRa.end();
  } else if (str.startsWith("x"))  {
    Serial.println("Lora begin");
    mode_set = 1;
    LoRa.begin(BAND);

  } else if (str.startsWith("t"))  {
    if (mode_set == 1) {
      String send_str = test_data + flag;
      Serial.println(send_str);
      LoRa.beginPacket();
      LoRa.print(send_str);
      LoRa.endPacket();
      flag++;
      if (flag > 100) {
        flag = 0;
      }
    }
  }

}
