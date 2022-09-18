//백업용 파일 20210307//
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 4); // RX, TX

uint8_t D[] = {0x03, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};

int dataChar[10];

float t_data = 28.7;
float f_data = 326;
float p_data = 141.5;


float t_data1 = 28.7;
float p_data1 = 141.5;
float f_data1 = 326;

float t_data2 = 32.6;
float p_data2 = 119.7;
float f_data2 = 712;

float t_data3 = 15.4;
float p_data3 = 121.9;
float f_data3 = 123;

int mode = 1;

long flag = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Start 485 data Generator");
}

void loop() {
  // put your main code here, to run repeatedly:


//  delay(100);
//  int sizeofD = sizeof(D);
//  for (int i = 0; i < sizeofD; i++) {
//    Serial.print(D[i],HEX);
//    mySerial.write(D[i]);
//  }
//  Serial.println();
  
  if (mySerial.available()){
    readSerial2();  
    delay(10);
    if(dataChar[0] == 1){
      Serial.print("Send Pressure : ");
      int int_data = (int)(p_data * 10);
      D[0] = 0x01;
      D[3] = (int)(int_data / 256);
      D[4] = (int)(int_data % 256);
      D[6] =  0x01;
      int sizeofD = sizeof(D);
      for (int i = 0; i < sizeofD; i++) {
        Serial.print(D[i],HEX);
        mySerial.write(D[i]);
      }
      Serial.println();
    } else if(dataChar[0] == 2){
      Serial.print("Send flow(");
      Serial.print(f_data);
      Serial.print(") : ");

      int int_data = (int)(f_data * 10);
      D[0] = 0x02;
      D[3] = (int)(int_data / 256);
      D[4] = (int)(int_data % 256);
      D[6] =  0x01;
      int sizeofD = sizeof(D);
      for (int i = 0; i < sizeofD; i++) {
        Serial.print(D[i],HEX);
        mySerial.write(D[i]);
      }
      Serial.println();
    } else if(dataChar[0] == 3){
      Serial.print("Send Temp(");
      Serial.print(t_data);
      Serial.print(") : ");
      int int_data = (int)(t_data * 10);
      D[0] = 0x03;
      D[3] = (int)(int_data / 256);
      D[4] = (int)(int_data % 256);
      D[6] =  0x01;
      int sizeofD = sizeof(D);
      for (int i = 0; i < sizeofD; i++) {
        Serial.print(D[i],HEX);
        mySerial.write(D[i]);
      }
      Serial.println();
    }
    
  }

  if(flag > 4999){ // 5초마다 값 변경
    flag = 0;
    if(mode==1){
      mode = 2;
      t_data = t_data2;
      p_data = p_data2;
      f_data = f_data2;
    } else if(mode==2){
      mode = 3;
      t_data = t_data3;
      p_data = p_data3;
      f_data = f_data3;
    } else {
      mode = 1;
      t_data = t_data1;
      p_data = p_data1;
      f_data = f_data1;
    }
    
  }
  
  delay(1);
  flag ++;
}


String readSerial2(){   
   String str = "";   
   char ch;   
   int ct = 0;
   Serial.print("Recived : "); 
    while( mySerial.available() > 0 )   
    {   
      ch = mySerial.read();   
      str.concat(ch);
      Serial.print(" "); 
      Serial.print(ch,HEX); 
      dataChar[ct] = (int) ch; 
      if(dataChar[ct] < 0){
        dataChar[ct] = 256 - (int)(0xFFFFFF - ch);
       }
      ct++;
      delay(5);  
    }   
    Serial.print(" => "); 
    //Serial.println();
    return str;     
} 
